project(
    'liftesch',
    'c',
    default_options: [
        'c_std=c99',
    ],
)

sdl = dependency('SDL2', required: true)
sdl_image = dependency('SDL2_image', required: true)
sdl_gfx = dependency('SDL2_gfx', required: true)
sdl_mixer = dependency('SDL2_mixer', required: true)

includes = include_directories('src')

sources = files(
    'src/adat.c',
    'src/elvono.c',
    'src/grafika.c',
    'src/liftmat.c',
)

executable(
    'liftesch',
    sources,
    dependencies: [sdl, sdl_image, sdl_gfx, sdl_mixer],
    include_directories: includes,
)