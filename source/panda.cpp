#include <Python.h>

#include <stdio.h>

#include <pandagles2.h>
#include <pandaexpress.h>
#include <panda.h>
#include <direct.h>
#include <config_openalAudio.h>
#include <config_pgraphnodes.h>
#include <config_cull.h>
#include <load_prc_file.h>

extern "C"
{
	PyObject* PyInit_core();
	PyObject* PyInit_ode();
	PyObject* PyInit_physics();
	PyObject* PyInit_direct();
}

void panda_main(int argc, char* argv[]) {

	std::ifstream t("romfs:/etc/20_panda.prc");
	std::stringstream buffer;
	buffer << t.rdbuf();

	load_prc_file_data("20_panda.prc", buffer.str().c_str());

	init_libpgraphnodes();
	init_libpandaexpress();
	init_libpanda();
	init_libcull();
	init_libpandagles2();
	init_libOpenALAudio();
	init_libdirect();

    PyObject *panda3d_module = PyModule_New("panda3d");
    PyObject_SetAttrString(panda3d_module, "__package__", PyUnicode_FromString("panda3d"));
    PyObject_SetAttrString(panda3d_module, "__path__", PyList_New(0));
    PyDict_SetItemString(PyImport_GetModuleDict(), "panda3d", panda3d_module);

    const char *oldcontext = _Py_PackageContext;

#define INIT_PANDA_MODULE(name) \
    _Py_PackageContext = (char *)("panda3d." #name); \
    do { \
        PyObject* m = PyInit_ ## name(); \
        PyDict_SetItemString(PyImport_GetModuleDict(), \
                             "panda3d." #name, m); \
        Py_XINCREF(m); \
        PyModule_AddObject(panda3d_module, #name, m); \
    } while(0)

    INIT_PANDA_MODULE(core);
    INIT_PANDA_MODULE(direct);
    INIT_PANDA_MODULE(physics);
    INIT_PANDA_MODULE(ode);

    _Py_PackageContext = oldcontext;
    Py_DECREF(panda3d_module);


}
