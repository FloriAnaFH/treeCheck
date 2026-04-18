#pragma once

#include "tree.h"
#include <filesystem>


void analysis_mode ( const std::filesystem::path& file );

void search_mode ( const std::filesystem::path& searchTree, const std::filesystem::path& subTree );
