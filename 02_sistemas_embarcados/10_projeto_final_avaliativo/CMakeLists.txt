# Define a versão mínima do CMake
cmake_minimum_required(VERSION 3.16)

# Inclui a funcionalidade padrão do ESP-IDF
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Define o nome do seu projeto
project(detector_de_gas)