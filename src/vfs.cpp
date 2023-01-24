#include "vfs.hpp"

inline constexpr auto self = "./";
inline constexpr auto prev = "../";

void vfs::unpackFiles(rapidjson::Value &object,
                      const std::string &sPath) noexcept {


    for (auto itr = object.MemberBegin(); itr != object.MemberEnd(); ++itr) {
        std::string sFilePath = sPath + itr->name.GetString();
        rapidjson::Value &vMember = itr->value;
        if (vMember.IsObject()) {
            if (vMember.HasMember("files")) {

                unpackFiles(vMember["files"], sFilePath + DIR_SEPARATOR);
            } else {
                if (!(vMember.HasMember("size") &&
                      vMember.HasMember("offset") && vMember["size"].IsInt() &&
                      vMember["offset"].IsString()))
                    continue;

                if (!m_extract) { continue; }

                size_t uSize = vMember["size"].GetUint();
                int uOffset = std::stoi(vMember["offset"].GetString());

                std::string res(uSize, ' ');
                m_ifsInputFile.seekg(m_headerSize + uOffset);
                m_ifsInputFile.read(&res[0], uSize);

                vfs_ptr->insert(fs_pair(sFilePath, res));
            }
        }
    }
}

bool vfs::readArchive(const std::string &filename) noexcept {

    m_ifsInputFile.open(filename, std::ios::binary);
    if (!m_ifsInputFile) { return false; }

    char sizeBuf[8];
    m_ifsInputFile.read(sizeBuf, 8);
    uint32_t uSize = *(uint32_t *)(sizeBuf + 4) - 8;

    m_headerSize = uSize + 16;
    char *headerBuf = new char[uSize + 1];
    m_ifsInputFile.seekg(16);
    m_ifsInputFile.read(headerBuf, uSize);
    headerBuf[uSize] = 0;

    rapidjson::Document json;
    rapidjson::ParseResult res = json.Parse(headerBuf);
    if (!res) { return false; }

    unpackFiles(json["files"]);
    m_szOffset = 0;

    m_ifsInputFile.close();

    delete[] headerBuf;

    return true;
}

void vfs::unpackFilesMem(rapidjson::Value &object, std::uint8_t *buf,
                         const std::string &sPath) noexcept {

    for (auto itr = object.MemberBegin(); itr != object.MemberEnd(); ++itr) {
        std::string sFilePath = sPath + itr->name.GetString();
        rapidjson::Value &vMember = itr->value;
        if (vMember.IsObject()) {
            if (vMember.HasMember("files")) {

                unpackFilesMem(vMember["files"], buf,
                               sFilePath + DIR_SEPARATOR);
            } else {
                if (!(vMember.HasMember("size") &&
                      vMember.HasMember("offset") && vMember["size"].IsInt() &&
                      vMember["offset"].IsString()))
                    continue;

                if (!m_extract) { continue; }

                size_t uSize = vMember["size"].GetUint();
                int uOffset = std::stoi(vMember["offset"].GetString());

                std::string res(uSize, ' ');
                memcpy(&res[0], buf + (m_headerSize + uOffset), uSize);

                vfs_ptr->insert(fs_pair(sFilePath, res));
            }
        }
    }
}

vfs::vfs() noexcept {

    vfs_ptr->insert(fs_pair(self, vfs_ptr));
    vfs_ptr->insert(fs_pair(prev, vfs_ptr));
    cur_dir = vfs_ptr;
}

bool vfs::readArchive(std::uint8_t *buf) noexcept {

    char sizeBuf[8];
    memcpy(sizeBuf, buf, 8);
    uint32_t uSize = *(uint32_t *)(sizeBuf + 4) - 8;

    m_headerSize = uSize + 16;
    char *headerBuf = new char[uSize + 1];
    memcpy(headerBuf, buf + 16, uSize);
    headerBuf[uSize] = 0;

    rapidjson::Document json;
    rapidjson::ParseResult res = json.Parse(headerBuf);
    if (!res) { return false; }

    unpackFilesMem(json["files"], buf);
    m_szOffset = 0;

    delete[] headerBuf;

    return true;
}

std::unique_ptr<std::vector<std::string>>
    vfs::parsePath(const std::string &path) noexcept {

    std::unique_ptr<std::vector<std::string>> ret =
        std::make_unique<std::vector<std::string>>();
    std::string tmp;
    std::uint64_t i = 0;

    if (path[0] == '/') ++i;

    for (i; i < path.length(); ++i) {

        tmp.push_back(path[i]);

        if (path[i] == DIR_SEPARATOR) {

            ret->push_back(tmp);
            tmp.clear();
        }
    }
    if (tmp.length() > 0) ret->push_back(tmp);

    return ret;
}

bool vfs::exists(const std::string &filename) noexcept {

    if (filename.length() == 0) return false;

    std::shared_ptr<fs_map> tmp = std::make_shared<fs_map>();

    if (filename[0] == DIR_SEPARATOR) tmp = vfs_ptr;
    else tmp = cur_dir;

    auto parsedPath = this->parsePath(filename);

    for (std::uint64_t i = 0; i < parsedPath->size(); ++i) {

        auto it = tmp->find(parsedPath->at(i));
        if (it != tmp->end()) {

            if (i + 1 == parsedPath->size()) return true;

            tmp = std::any_cast<std::shared_ptr<fs_map>>(it->second);
        } else return false;
    }

    return true;
}

bool vfs::remove(const std::string &path) noexcept {

    if (path.length() == 0) return false;

    std::shared_ptr<fs_map> tmp = std::make_shared<fs_map>();

    if (path[0] == DIR_SEPARATOR) tmp = vfs_ptr;
    else tmp = cur_dir;

    auto parsedPath = this->parsePath(path);

    for (std::uint64_t i = 0; i < parsedPath->size(); ++i) {

        auto it = tmp->find(parsedPath->at(i));
        if (it != tmp->end() && (i + 1) == parsedPath->size()) {

            tmp->erase(parsedPath->at(i));
            return true;

        } else if (it != tmp->end())
            tmp = std::any_cast<std::shared_ptr<fs_map>>(it->second);
        else break;
    }

    return false;
}

bool vfs::cd(const std::string &path) noexcept {

    if (path.length() == 0) return false;

    std::shared_ptr<fs_map> tmp = std::make_shared<fs_map>();

    if (path[0] == DIR_SEPARATOR) tmp = vfs_ptr;
    else tmp = cur_dir;

    auto parsedPath = this->parsePath(path);

    for (auto &entry : *parsedPath) {

        auto it = tmp->find(entry);
        if (it != tmp->end()) {

            tmp = std::any_cast<std::shared_ptr<fs_map>>(it->second);
        } else return false;
    }

    cur_dir = tmp;
    return true;
}

bool vfs::createDir(const std::string &dirname) noexcept {

    if (dirname.length() == 0) return false;

    std::shared_ptr<fs_map> tmp = std::make_shared<fs_map>();

    if (dirname[0] == DIR_SEPARATOR) tmp = vfs_ptr;
    else tmp = cur_dir;

    auto parsedPath = this->parsePath(dirname);

    for (std::uint64_t i = 0; i < parsedPath->size(); ++i) {

        auto it = tmp->find(parsedPath->at(i));
        if (it == tmp->end() && (i + 1) == parsedPath->size()) {

            std::shared_ptr<fs_map> directory = std::make_shared<fs_map>();
            directory->insert(fs_pair(self, directory));
            directory->insert(fs_pair(prev, tmp));

            if (parsedPath->at(i)[parsedPath->at(i).length() - 1] !=
                DIR_SEPARATOR)
                return false;

            tmp->insert(fs_pair(parsedPath->at(i), directory));
            return true;

        } else if (it != tmp->end())
            tmp = std::any_cast<std::shared_ptr<fs_map>>(it->second);
        else break;
    }

    return false;
}

std::unique_ptr<std::vector<std::string>>
    vfs::listDir(const std::string &dirname) noexcept {

    if (dirname.length() == 0) return {};

    std::shared_ptr<fs_map> tmp = std::make_shared<fs_map>();
    auto ret_ptr = std::make_unique<std::vector<std::string>>();

    if (dirname[0] == DIR_SEPARATOR) tmp = vfs_ptr;
    else tmp = cur_dir;
    auto parsedPath = this->parsePath(dirname);

    for (auto &entry : *parsedPath) {

        auto it = tmp->find(entry);
        if (it != tmp->end()) {

            tmp = std::any_cast<std::shared_ptr<fs_map>>(it->second);
        } else break;
    }

    for (const auto &[key, val] : *tmp) ret_ptr->push_back(key);

    return ret_ptr;
}

bool vfs::createSymlink(const std::string &path_from,
                        const std::string &path_to) noexcept {
}

/*

        bool createSymlink(const std::string &path_from,
                           const std::string &path_to) noexcept;

        bool isDirectory(const std::string &dirname) noexcept;
};

*/