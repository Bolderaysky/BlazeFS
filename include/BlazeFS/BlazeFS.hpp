#pragma once

#include <memory>
#include <vector>
#include <fstream>
#include <string>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "BlazeFS/internal/types.hpp"

namespace blazefs {

    class BlazeFS {

        protected:
            // Variables

            std::uint64_t m_headerSize = 0;
            std::uint64_t m_szOffset = 0;
            bool m_extract = true;

            std::ifstream m_ifsInputFile;

            std::shared_ptr<blazefs::internal::fs_map> vfs_ptr =
                std::make_shared<blazefs::internal::fs_map>();
            std::shared_ptr<blazefs::internal::fs_map> cur_dir;

            void unpackFiles(rapidjson::Value &object,
                             const std::string &sPath = "/") noexcept;

            void unpackFilesMem(rapidjson::Value &object, std::uint8_t *buf,
                                const std::string &sPath = "/") noexcept;

            blazefs::internal::parsedPathObject
                parsePath(const std::string &path) noexcept;

            blazefs::internal::parsedPathObject
                parseMkdirPath(const std::string &path) noexcept;

            bool mkdirWrite(const std::string &filename,
                            const std::string &value) noexcept;

        public:
            // Default constructor
            BlazeFS() noexcept;
            // Constructor which allows to reserve memory in / at the moment of
            // class object creation
            explicit BlazeFS(std::uint32_t chunk) noexcept;

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

            // Create link from one directory to another. Directory 'name'
            // will point to the directory 'target'
            bool createSymlink(const std::string &name,
                               const std::string &target) noexcept;

            // Check if certain file is directory or not
            bool isDirectory(const std::string &dirname) noexcept;

            // Allows to reserve memory in certain directory if you want to
            // insert a lot of elements which can lead to significant
            // performance improvement
            bool reserve(const std::string &dirname,
                         std::uint32_t size) noexcept;
    };


    /*

        Implementations of template methods

    */

    template <typename T>
    bool BlazeFS::write(const std::string &filename, const T &value) noexcept {

        if (filename.empty()) return false;

        auto tmp = parsePath(filename);

        if (!tmp.status) return false;

        auto [it, success] = tmp.ptr->try_emplace(tmp.lastElement, value);
        if (!success) it.value() = value;

        return true;
    }

    template <typename T>
    T BlazeFS::read(const std::string &filename) noexcept {
        if (filename.empty()) return T();

        auto tmp = parsePath(filename);
        if (!tmp.status) return T();

        auto it = tmp.ptr->find(tmp.lastElement);
        if (it == tmp.ptr->end()) return T();

        return std::any_cast<T>(it->second);
    }

}; // namespace blazefs