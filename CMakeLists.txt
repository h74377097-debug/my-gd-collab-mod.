cmake_minimum_required(VERSION 3.25)

project(GD_Collab_Editor)

# Добавляем Geode SDK
find_package(Geode REQUIRED)

# Указываем файлы исходного кода
add_geode_mod(
    ${PROJECT_NAME}
    GD_COLLAB_MOD
    main.cpp
)

# Опции линковки для работы с сетью (WebSockets)
if(GEODE_TARGET_PLATFORM_WINDOWS)
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)
endif()
