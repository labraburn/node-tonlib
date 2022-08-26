{
    "targets": [{
        "target_name":
        "tonlibjson",
        "cflags!": ["-fno-exceptions"],
        "cflags_cc!": ["-fno-exceptions"],
        "sources": ["source/main.cpp", "source/tonlib.cpp"],
        'include_dirs': [
            "<!@(node -p \"require('node-addon-api').include\")",
        ],
        'defines': ['NAPI_DISABLE_CPP_EXCEPTIONS'],
        "libraries": [
            "-Wl,-rpath,<(module_root_dir)"
        ],
    }]
}