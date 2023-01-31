#pragma once

#include <cstdlib>
#include <array>
#include <any>

#include "tsl/bhopscotch_map.h"

namespace blazefs {

    namespace internal {

        using fs_pair = std::pair<std::string, std::any>;
        using fs_map = tsl::bhopscotch_map<std::string, std::any>;

        struct parsedPathObject {

                bool status;
                std::shared_ptr<fs_map> ptr;
                std::string lastElement;
        };


    }; // namespace internal

}; // namespace blazefs