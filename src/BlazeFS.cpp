#include "blaze/BlazeFS/BlazeFS.hpp"

// #include "glaze/glaze.hpp"

#define DIR_SEPARATOR '/'

inline constexpr auto self = "./";
inline constexpr auto prev = "../";

namespace blaze {


    void BlazeFS::unpackFiles(rapidjson::Value &object,
                              const std::string &sPath) noexcept {


        for (auto itr = object.MemberBegin(); itr != object.MemberEnd();
             ++itr) {
            std::string sFilePath = sPath + itr->name.GetString();
            rapidjson::Value &vMember = itr->value;
            if (vMember.IsObject()) {
                if (vMember.HasMember("files")) {

                    unpackFiles(vMember["files"], sFilePath + DIR_SEPARATOR);
                } else {
                    if (!(vMember.HasMember("size") &&
                          vMember.HasMember("offset") &&
                          vMember["size"].IsInt() &&
                          vMember["offset"].IsString()))
                        continue;

                    if (!m_extract) { continue; }

                    size_t uSize = vMember["size"].GetUint();
                    int uOffset = std::stoi(vMember["offset"].GetString());

                    std::string res(uSize, ' ');
                    m_ifsInputFile.seekg(m_headerSize + uOffset);
                    m_ifsInputFile.read(&res[0], uSize);

                    this->mkdirWrite(sFilePath, res);
                }
            }
        }
    }


    void BlazeFS::unpackFilesMem(rapidjson::Value &object, std::uint8_t *buf,
                                 const std::string &sPath) noexcept {

        for (auto itr = object.MemberBegin(); itr != object.MemberEnd();
             ++itr) {
            std::string sFilePath = sPath + itr->name.GetString();
            rapidjson::Value &vMember = itr->value;
            if (vMember.IsObject()) {
                if (vMember.HasMember("files")) {

                    unpackFilesMem(vMember["files"], buf,
                                   sFilePath + DIR_SEPARATOR);
                } else {
                    if (!(vMember.HasMember("size") &&
                          vMember.HasMember("offset") &&
                          vMember["size"].IsInt() &&
                          vMember["offset"].IsString()))
                        continue;

                    if (!m_extract) { continue; }

                    size_t uSize = vMember["size"].GetUint();
                    int uOffset = std::stoi(vMember["offset"].GetString());

                    std::string res(uSize, ' ');
                    memcpy(&res[0], buf + (m_headerSize + uOffset), uSize);

                    this->mkdirWrite(sFilePath, res);
                }
            }
        }
    }

    bool BlazeFS::readArchive(const std::string &filename) noexcept {

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

        this->unpackFiles(json["files"]);
        m_szOffset = 0;

        m_ifsInputFile.close();

        delete[] headerBuf;

        return true;
    }

    bool BlazeFS::readArchive(std::uint8_t *buf) noexcept {

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

        this->unpackFilesMem(json["files"], buf);
        m_szOffset = 0;

        delete[] headerBuf;

        return true;
    }

    bool BlazeFS::mkdirWrite(const std::string &filename,
                             const std::string &value) noexcept {

        if (filename.empty()) return false;

        auto tmp = parseMkdirPath(filename);

        if (!tmp.status) return false;

        auto [it, success] = tmp.ptr->try_emplace(tmp.lastElement, value);
        if (!success) it.value() = value;

        return true;
    }

    BlazeFS::BlazeFS(std::uint32_t chunk) noexcept {

        vfs_ptr->reserve(chunk);
        vfs_ptr->emplace(self, vfs_ptr);
        vfs_ptr->emplace(prev, vfs_ptr);
        cur_dir = vfs_ptr;
    }

    BlazeFS::BlazeFS() noexcept {

        vfs_ptr->emplace(self, vfs_ptr);
        vfs_ptr->emplace(prev, vfs_ptr);
        cur_dir = vfs_ptr;
    }

    blaze::internal::parsedPathObject
        BlazeFS::parseMkdirPath(const std::string &path) noexcept {

        blaze::internal::parsedPathObject ret;

        std::uint16_t i = 0;

        if (path[0] == DIR_SEPARATOR) {
            ++i;
            ret.ptr = vfs_ptr;
        } else ret.ptr = cur_dir;

        std::string tmp;
        tmp.reserve(256);

        for (; i < path.length() - 1; ++i) {

            tmp.push_back(path[i]);

            if (path[i] == DIR_SEPARATOR) {

                auto it = ret.ptr->find(tmp);
                if (it != ret.ptr->end())
                    ret.ptr =
                        std::any_cast<std::shared_ptr<blaze::internal::fs_map>>(
                            it->second);
                else {

                    std::shared_ptr<blaze::internal::fs_map> directory =
                        std::make_shared<blaze::internal::fs_map>();
                    directory->emplace(self, directory);
                    directory->emplace(prev, ret.ptr);

                    ret.ptr->emplace(tmp, directory);

                    ret.ptr = directory;
                }
                tmp.clear();
            }
        }

        tmp.push_back(path[i]);

        ret.lastElement = tmp;

        ret.status = true;

        return ret;
    }

    blaze::internal::parsedPathObject
        BlazeFS::parsePath(const std::string &path) noexcept {

        blaze::internal::parsedPathObject ret;

        std::uint16_t i = 0;

        if (path[0] == DIR_SEPARATOR) {
            ++i;
            ret.ptr = vfs_ptr;
        } else ret.ptr = cur_dir;

        std::string tmp;
        tmp.reserve(256);

        for (; i < path.length() - 1; ++i) {

            tmp.push_back(path[i]);

            if (path[i] == DIR_SEPARATOR) {

                auto it = ret.ptr->find(tmp);
                if (it != ret.ptr->end())
                    ret.ptr =
                        std::any_cast<std::shared_ptr<blaze::internal::fs_map>>(
                            it->second);
                else {

                    ret.status = false;
                    return ret;
                }
                tmp.clear();
            }
        }

        tmp.push_back(path[i]);

        ret.lastElement = tmp;

        ret.status = true;

        return ret;
    }

    bool BlazeFS::exists(const std::string &filename) noexcept {

        if (filename.empty()) return false;

        auto tmp = this->parsePath(filename);

        if (!tmp.status) return false;

        auto it = tmp.ptr->find(tmp.lastElement);
        if (it == tmp.ptr->end()) return false;

        return true;
    }

    bool BlazeFS::remove(const std::string &path) noexcept {

        if (path.empty()) return false;

        auto tmp = this->parsePath(path);

        if (!tmp.status) return false;

        auto it = tmp.ptr->find(tmp.lastElement);
        if (it == tmp.ptr->end()) return false;

        tmp.ptr->erase(tmp.lastElement);

        return true;
    }

    bool BlazeFS::cd(const std::string &path) noexcept {

        if (path.empty()) return false;

        if (path == "/") {

            cur_dir = vfs_ptr;
            return true;
        }

        auto tmp = this->parsePath(path);

        if (!tmp.status) return false;

        auto it = tmp.ptr->find(tmp.lastElement);
        if (it == tmp.ptr->end()) return false;

        cur_dir = std::any_cast<std::shared_ptr<blaze::internal::fs_map>>(
            it->second);

        return true;
    }

    bool BlazeFS::createDir(const std::string &dirname) noexcept {


        if (dirname.empty() || dirname == "/") return false;


        auto tmp = this->parsePath(dirname);

        if (!tmp.status) return false;

        if (tmp.ptr->contains(tmp.lastElement)) return false;

        std::shared_ptr<blaze::internal::fs_map> directory =
            std::make_shared<blaze::internal::fs_map>();
        directory->emplace(self, directory);
        directory->emplace(prev, tmp.ptr);

        tmp.ptr->emplace(tmp.lastElement, directory);

        return true;
    }

    std::unique_ptr<std::vector<std::string>>
        BlazeFS::listDir(const std::string &dirname) noexcept {

        if (dirname.empty()) return {};

        blaze::internal::parsedPathObject tmp;
        if (dirname != "/") {

            tmp = this->parsePath(dirname);

            if (!tmp.status) return {};

            auto it = tmp.ptr->find(tmp.lastElement);
            if (it == tmp.ptr->end()) return {};

            tmp.ptr = std::any_cast<std::shared_ptr<blaze::internal::fs_map>>(
                it->second);
        } else tmp.ptr = vfs_ptr;

        auto ret = std::make_unique<std::vector<std::string>>();
        ret->reserve(2048);

        for (const auto &[key, val] : *tmp.ptr) ret->push_back(key);

        ret->shrink_to_fit();

        return ret;
    }

    bool BlazeFS::createSymlink(const std::string &name,
                                const std::string &target) noexcept {


        if (name.empty() || target.empty()) return false;

        auto tmp_from = this->parsePath(name);
        auto tmp_to = this->parsePath(target);

        if (!tmp_from.status || !tmp_to.status) return false;

        auto it_from = tmp_from.ptr->find(tmp_from.lastElement);
        if (it_from != tmp_from.ptr->end()) return false;

        auto it_to = tmp_to.ptr->find(tmp_to.lastElement);
        if (it_to == tmp_to.ptr->end()) return false;

        tmp_from.ptr->emplace(
            tmp_from.lastElement,
            std::any_cast<std::shared_ptr<blaze::internal::fs_map>>(
                it_to->second));

        return true;
    }

    bool BlazeFS::isDirectory(const std::string &dirname) noexcept {

        if (dirname.empty()) return false;

        auto tmp = this->parsePath(dirname);

        if (!tmp.status) return false;

        auto it = tmp.ptr->find(tmp.lastElement);
        if (it == tmp.ptr->end()) return false;

        if (tmp.lastElement[tmp.lastElement.length() - 1] != DIR_SEPARATOR)
            return false;

        return true;
    }

    bool BlazeFS::reserve(const std::string &dirname,
                          std::uint32_t size) noexcept {

        if (dirname.empty()) return false;

        if (dirname == "/") {

            vfs_ptr->reserve(size);
            return true;
        }

        auto tmp = this->parsePath(dirname);

        if (!tmp.status) return false;

        auto it = tmp.ptr->find(tmp.lastElement);
        if (it == tmp.ptr->end()) return false;

        if (tmp.lastElement[tmp.lastElement.length() - 1] != DIR_SEPARATOR)
            return false;

        std::any_cast<std::shared_ptr<blaze::internal::fs_map>>(it->second)
            ->reserve(size);
        return true;
    }
}; // namespace blaze