#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <any>

#include "rapidjson/document.h"
#include "rapidjson/error/en.h"

using fs_pair = std::pair<std::string, std::any>;
using fs_map = std::unordered_map<std::string, std::any>;

#define DIR_SEPARATOR '/'

class vfs {

    protected:
        // Variables
        std::shared_ptr<fs_map> vfs_ptr = std::make_shared<fs_map>();
        std::shared_ptr<fs_map> cur_dir = vfs_ptr;
        std::ifstream m_ifsInputFile;
        std::uint64_t m_headerSize = 0;
        std::uint64_t m_szOffset = 0;
        bool m_extract = true;

        // Protected methods
        void unpackFiles(rapidjson::Value &object,
                         const std::string &sPath = "/") noexcept;
        void unpackFilesMem(rapidjson::Value &object, std::uint8_t *buf,
                            const std::string &sPath = "/") noexcept;

        std::unique_ptr<std::vector<std::string>>
            parsePath(const std::string &path) noexcept;


    public:
        vfs() noexcept;

        // Read archive from file. Takes path to the file as argument
        bool readArchive(const std::string &filename) noexcept;
        // Read archive from memory. Takes buffer as argument
        bool readArchive(std::uint8_t *buf) noexcept;

        // Read content of file from virtual filesystem
        template <typename T> T read(const std::string &filename) noexcept;
        // Check if certain file or directory exists
        bool exists(const std::string &filename) noexcept;
        // Remove file or directory from virtual filesystem
        bool remove(const std::string &path) noexcept;
        // Returns std::vector that contains all file and directory names
        // from current directory
        std::unique_ptr<std::vector<std::string>>
            listDir(const std::string &dirname) noexcept;

        // write 'value' into 'filename'
        template <typename T>
        bool write(const std::string &filename, const T &value) noexcept;

        // Change current directory
        bool cd(const std::string &path) noexcept;
        // Create new directory
        bool createDir(const std::string &dirname) noexcept;

        // Create link from one directory to another. Directory 'path_from' will
        // point to the directory 'path_to'
        bool createSymlink(const std::string &path_from,
                           const std::string &path_to) noexcept;

        // Check if certain file is directory or not
        bool isDirectory(const std::string &dirname) noexcept;
};


/*

    Implementations of template methods

*/

template <typename T>
bool vfs::write(const std::string &filename, const T &value) noexcept {

    if (filename.length() == 0) return false;

    std::shared_ptr<fs_map> tmp = std::make_shared<fs_map>();

    if (filename[0] == DIR_SEPARATOR) tmp = vfs_ptr;
    else tmp = cur_dir;

    auto parsedPath = this->parsePath(filename);

    for (std::uint64_t i = 0; i < parsedPath->size(); ++i) {

        auto it = tmp->find(parsedPath->at(i));
        if (it == tmp->end() && (i + 1) == parsedPath->size()) {

            tmp->insert(fs_pair(parsedPath->at(i), value));
            return true;

        } else if (it != tmp->end() && (i + 1) == parsedPath->size()) {

            it->second = value;
            return true;

        } else if (it != tmp->end())
            tmp = std::any_cast<std::shared_ptr<fs_map>>(it->second);
        else break;
    }

    return false;
}

template <typename T> T vfs::read(const std::string &filename) noexcept {

    if (filename.length() == 0) return "";

    std::shared_ptr<fs_map> tmp = std::make_shared<fs_map>();

    if (filename[0] == DIR_SEPARATOR) tmp = vfs_ptr;
    else tmp = cur_dir;
    auto parsedPath = this->parsePath(filename);

    for (std::uint64_t i = 0; i < parsedPath->size(); ++i) {

        auto it = tmp->find(parsedPath->at(i));
        if (it != tmp->end() && (i + 1) == parsedPath->size()) {

            return std::any_cast<T>(tmp->at(parsedPath->at(i)));

        } else if (it != tmp->end())
            tmp = std::any_cast<std::shared_ptr<fs_map>>(it->second);
        else break;
    }

    return "";
}