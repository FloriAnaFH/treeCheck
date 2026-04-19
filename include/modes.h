#pragma once

#include "tree.h"
#include <filesystem>


void analysisMode ( const std::filesystem::path& file, bool rebalance = false );

void searchMode ( const std::filesystem::path& searchTree, const std::filesystem::path& subTree, bool rebalance = false );
