#define PY_SSIZE_T_CLEAN
#include <Python.h>

// Include the most common headers from the C standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

// Include the main libnx system header, for Switch development
#include <switch.h>

#include "nxpanda.h"

#define MAINPY "sdmc:/main.py"

// Main program entrypoint
int main(int argc, char* argv[])
{

    int re = 0;
    FILE* mainpy = NULL;

//    consoleInit(NULL);
    consoleDebugInit(debugDevice_SVC);

    // Configure our supported input layout: a single player with standard controller styles
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);

    // Initialize the default gamepad (which reads handheld mode inputs as well as the first connected controller)
    PadState pad;
    padInitializeDefault(&pad);

    socketInitializeDefault();

    Result rc = romfsInit();
    if (R_FAILED(rc))
        return 1;

    PyStatus status;

    PyConfig config;
    PyConfig_InitIsolatedConfig(&config);

    config.site_import = 0;
    chdir("romfs:/");

    status = PyWideStringList_Append(&config.module_search_paths, L"lib/python3.10");
    if (PyStatus_Exception(status)) {
	    goto pyerr;
    }
 
    status = Py_InitializeFromConfig(&config);
    if (PyStatus_Exception(status)) {
	    goto pyerr;
    }

    // Initialize Panda3D 
    panda_main(argc, argv);

    mainpy = fopen(MAINPY, "r");

    if (mainpy == NULL) {
            fprintf(stderr, "Error: could not open " MAINPY "\n");
    } else {
            PyRun_AnyFile(mainpy, MAINPY);
	    fprintf(stderr, "Pythen executed\n");
    }

    // Deinitialize and clean up resources used by the console (important!)
    socketExit();

    return 0;
pyerr:

    PyConfig_Clear(&config);

    if (PyStatus_IsExit(status)) {
	    return status.exitcode;
    }

    /* TODO display this on screen */

    Py_ExitStatusException(status);
}
