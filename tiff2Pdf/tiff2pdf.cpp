/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include "tiffWriter/image.hpp"
#include "tiffWriter/traitsAndPolicies.hpp"
#include "libLiFFT/traits/IdentityAccessor.hpp"
#include "libLiFFT/policies/Copy.hpp"
#include "libLiFFT/types/AddDimsWrapper.hpp"
#include "libLiFFT/accessors/StreamAccessor.hpp"

std::string
remove_extension(const std::string& filename) {
    size_t lastdot = filename.find_last_of(".");
    if (lastdot == std::string::npos) return filename;
    return filename.substr(0, lastdot);
}

int main(int argc, char** argv) {
    if(argc != 3 && argc != 2){
        std::cerr << "Usage: " << argv[0] << "<src.tif> [<dst.pdf>]" << std::endl;
        return 1;
    }
    std::string srcFilePath = argv[1];
    std::string destFilePath = (argc == 2) ? remove_extension(srcFilePath) + ".pdf" : argv[2];

    tiffWriter::FloatImage<> img(srcFilePath);
    std::string tmpFilePath = srcFilePath + ".txt";
    LiFFT::types::AddDimsWrapper< std::ofstream, 2 > outFile(tmpFilePath);
    LiFFT::policies::Copy< typename LiFFT::traits::IdentityAccessor<decltype(img)>::type, LiFFT::accessors::StringStreamAccessor<> > copy;

    copy(img, outFile);
    outFile.close();

    int result = 0;

    std::string cmd = "python writeData.py -s -i \"" + tmpFilePath + "\" -o \"" + destFilePath + "\"";
    if(std::system(cmd.c_str())){
        std::cerr << "Error converting txt to pdf" << std::endl;
        result = 2;
    }
    if(remove(tmpFilePath.c_str()) != 0){
        std::cerr << "Error removing temporary file: " << tmpFilePath << std::endl;
        result = 3;
    }

    return result;
}
