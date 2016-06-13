#include <Python.h>
#include <structmember.h>

#include "nxt.h"

#define COMPILING_IN_PY2 (PY_VERSION_HEX <= 0x03000000)
#if COMPILING_IN_PY2
#define PyLong_FromLong PyInt_FromLong
#endif /* COMPILING_IN_PY2 */

static int
validate_port(int port)
{
    if (port < 1 || port > 4) {
        PyErr_Format(PyExc_ValueError, "Port must be 1-4, got: %d", port);
        return -1;
    }
    return 0;
}

static int
validate_power(int power)
{
    if (power < -100 || power > 100) {
        PyErr_Format(PyExc_ValueError,
                     "Power must be in the range [-100, 100], got: %d",
                     power);
        return -1;
    }
    return 0;
}

typedef struct {
    PyObject_HEAD
    NXT nxt;
    unsigned char closed;
} nxtobject;

static int
check_closed(nxtobject *self) {
    if (self->closed) {
        PyErr_SetString(PyExc_IOError,
                        "Cannot perform operation on closed NXT connection.");
        return -1;
    }
    return 0;
}

static PyObject*
nxt_new(PyTypeObject *cls, PyObject *args, PyObject *kwargs)
{
    char *keywords[] = {"mac_address", NULL};
    char *mac_address;
    nxtobject *self;

    if (!PyArg_ParseTupleAndKeywords(args,
                                     kwargs,
                                     "s",
                                     keywords,
                                     &mac_address)) {
        return NULL;
    }

    if (!(self = (nxtobject*) cls->tp_alloc(cls, 0))) {
        return NULL;
    }

    if (NXT_init(&self->nxt) || NXT_connect(&self->nxt, mac_address)) {
        self->closed = 1;
        Py_XDECREF(self);
        PyErr_Format(PyExc_IOError,
                     "Failed to connect to a device at MAC: %s",
                     mac_address);
        return NULL;
    }

    self->closed = 0;
    return (PyObject*) self;
}

static void
nxt_dealloc(nxtobject *self)
{
    if (!self->closed) {
        NXT_destroy(&self->nxt);
    }
    PyObject_Del(self);
}

static PyObject*
nxt_repr(nxtobject *self) {
    return PyUnicode_FromFormat("<%s: %d%s>",
                                Py_TYPE(self)->tp_name,
                                self->nxt.dev_id,
                                (self->closed) ? " (closed)" : "");
}

PyDoc_STRVAR(nxt_play_tone_doc,
             "Play a tone of a given frequency for a certain amount of time\n"
             "on the NXT.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "freq : int\n"
             "    The frequency to play.\n"
             "time : int\n"
             "    The amount of time to play the note for in microsenconds.\n"
             "\n"
             "Raises\n"
             "------\n"
             "IOError\n"
             "    Raised when communication with the NXT fails.\n");

static PyObject*
nxt_play_tone(nxtobject *self, PyObject *args, PyObject *kwargs)
{
    char *keywords[] = {"freq", "time", NULL};
    unsigned short freq;
    unsigned short time;

    if (!PyArg_ParseTupleAndKeywords(args,
                                     kwargs,
                                     "HH",
                                     keywords,
                                     &freq,
                                     &time)) {
        return NULL;
    }

    if (check_closed(self)) {
        return NULL;
    }

    if (NXT_play_tone(&self->nxt, freq, time, 0, NULL)) {
        PyErr_SetString(PyExc_IOError, "Failed to play a tone");
        return NULL;
    }

    Py_RETURN_NONE;
}

PyDoc_STRVAR(nxt_stay_alive_doc,
             "Send a message to the NXT that prevents it from turning off.\n"
             "\n"
             "If the NXT doesn't see this message for a couple of minutes it\n"
             "will power down to save battery.\n"
             "\n"
             "Raises\n"
             "------\n"
             "IOError\n"
             "    Raised when communication with the NXT fails.\n");

static PyObject*
nxt_stay_alive(nxtobject *self, PyObject *_ __attribute__((unused)))
{
    if (check_closed(self)) {
        return NULL;
    }

    if (NXT_stay_alive(&self->nxt)) {
        PyErr_SetString(PyExc_IOError, "Failed to send stay_alve to the NXT");
        return NULL;
    }

    Py_RETURN_NONE;
}

PyDoc_STRVAR(nxt_init_button_doc,
             "Tell the NXT that there is a button plugged to a certain port.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "port : int\n"
             "    The port which has a button plugged in.\n"
             "\n"
             "Raises\n"
             "------\n"
             "ValueError\n"
             "    Raised when the port number is out of bounds.\n"
             "IOError\n"
             "    Raised when communication with the NXT fails.\n");

static PyObject*
nxt_init_button(nxtobject *self, PyObject *args, PyObject *kwargs)
{
    char *keywords[] = {"port", NULL};
    int port;

    if (!PyArg_ParseTupleAndKeywords(args,
                                     kwargs,
                                     "i",
                                     keywords,
                                     &port)) {
        return NULL;
    }

    if (port < 1 || port > 4) {
        PyErr_Format(PyExc_ValueError, "Port must be 1-4, got: %d", port);
        return NULL;
    }

    if (check_closed(self)) {
        return NULL;
    }

    /* Port is 0 indexed, 0 corrosponds to "Port 1" on the physical device. */
    if (NXT_initbutton(&self->nxt, (sensor_port) port - 1)) {
        PyErr_Format(PyExc_IOError,
                     "Failed to initalize the button on port %d",
                     port);
        return NULL;
    }

    Py_RETURN_NONE;
}

PyDoc_STRVAR(nxt_init_light_doc,
             "Tell the NXT that there is a light sensor plugged to a certain\n"
             "port.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "port : int\n"
             "    The port which has a light plugged in.\n"
             "\n"
             "Raises\n"
             "------\n"
             "ValueError\n"
             "    Raised when the port number is out of bounds.\n"
             "IOError\n"
             "    Raised when communication with the NXT fails.\n");

static PyObject*
nxt_init_light(nxtobject *self, PyObject *args, PyObject *kwargs)
{
    char *keywords[] = {"port", NULL};
    int port;

    if (!PyArg_ParseTupleAndKeywords(args,
                                     kwargs,
                                     "i",
                                     keywords,
                                     &port)) {
        return NULL;
    }

    if (validate_port(port)) {
        return NULL;
    }

    if (check_closed(self)) {
        return NULL;
    }

    /* Port is 0 indexed, 0 corrosponds to "Port 1" on the physical device. */
    if (NXT_initlight(&self->nxt, (sensor_port) port - 1)) {
        PyErr_Format(PyExc_IOError,
                     "Failed to initalize the light on port %d",
                     port);
        return NULL;
    }

    Py_RETURN_NONE;
}

PyDoc_STRVAR(nxt_is_pressed_doc,
             "Check if a button is currently pressed.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "port : int\n"
             "    The port of the button to check.\n"
             "\n"
             "Returns\n"
             "-------\n"
             "is_pressed : bool\n"
             "    Is the button currently being pressed?\n"
             "\n"
             "Raises\n"
             "------\n"
             "ValueError\n"
             "    Raised when the port number is out of bounds.\n"
             "IOError\n"
             "    Raised when communication with the NXT fails.\n");

static PyObject*
nxt_is_pressed(nxtobject *self, PyObject *args, PyObject *kwargs)
{
    char *keywords[] = {"port", NULL};
    int port;
    int result;

    if (!PyArg_ParseTupleAndKeywords(args,
                                     kwargs,
                                     "i",
                                     keywords,
                                     &port)) {
        return NULL;
    }

    if (validate_port(port)) {
        return NULL;
    }

    if (check_closed(self)) {
        return NULL;
    }

    if ((result = NXT_ispressed(&self->nxt, (sensor_port) port - 1)) < 0) {
        PyErr_Format(PyExc_IOError,
                     "Failed to read the state of the button on port %d",
                     port);
        return NULL;
    }

    return PyBool_FromLong(result);
}

PyDoc_STRVAR(nxt_read_light_doc,
             "Read the value of a light sensor.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "port : int\n"
             "    The port of the light sensor to read.\n"
             "\n"
             "Returns\n"
             "-------\n"
             "value : int\n"
             "    The value on a scale from 0 to 1024.\n"
             "\n"
             "Raises\n"
             "------\n"
             "ValueError\n"
             "    Raised when the port number is out of bounds.\n"
             "IOError\n"
             "    Raised when communication with the NXT fails.\n");

static PyObject*
nxt_read_light(nxtobject *self, PyObject *args, PyObject *kwargs)
{
    char *keywords[] = {"port", NULL};
    int port;
    int result;

    if (!PyArg_ParseTupleAndKeywords(args,
                                     kwargs,
                                     "i",
                                     keywords,
                                     &port)) {
        return NULL;
    }

    if (validate_port(port)) {
        return NULL;
    }

    if (check_closed(self)) {
        return NULL;
    }

    if ((result = NXT_ispressed(&self->nxt, (sensor_port) port - 1)) < 0) {
        PyErr_Format(PyExc_IOError,
                     "Failed to read the state of the light sensor on port %d",
                     port);
        return NULL;
    }

    return PyLong_FromLong(result);
}

#define DRIVE_FN(verb, direction)                                       \
    PyDoc_STRVAR(nxt_ ## verb ## _ ## direction ## _doc,                \
                 "Tell the nxt to " #verb " " #direction " for some\n"  \
                 "period of time at a specified power.\n"               \
                 "\n"                                                   \
                 "Parameters\n"                                         \
                 "----------\n"                                         \
                 "time : int\n"                                         \
                 "    The number of seconds to " #verb " for.\n"        \
                 "power : int\n"                                        \
                 "    How much power should be applied to the motors\n" \
                 "    [-100, 100].\n"                                   \
                 "left_port : int\n"                                    \
                 "    The port where the left motor is connected.\n"    \
                 "right_port : int\n"                                   \
                 "    The port where the right motor is connected.\n"   \
                 "\n"                                                   \
                 "Raises\n"                                             \
                 "------\n"                                             \
                 "ValueError\n"                                         \
                 "    Raised when the left or right port is out of bounds\n" \
                 "    or when the power is not in the range [-100, 100]\n" \
                 "IOError\n"                                            \
                 "    Raised when communication with the NXT fails.\n"); \
                                                                        \
    static PyObject*                                                    \
    nxt_ ## verb ## _ ## direction(nxtobject *self,                     \
                                   PyObject *args,                      \
                                   PyObject *kwargs)                    \
    {                                                                   \
        char *keywords[] = {"time",                                     \
                            "power",                                    \
                            "left_port",                                \
                            "right_port",                               \
                            NULL};                                      \
        int time;                                                       \
        int power;                                                      \
        int left_port;                                                  \
        int right_port;                                                 \
                                                                        \
        if (!PyArg_ParseTupleAndKeywords(args,                          \
                                         kwargs,                        \
                                         "iiii",                        \
                                         keywords,                      \
                                         &time,                         \
                                         &power,                        \
                                         &left_port,                    \
                                         &right_port)) {                \
            return NULL;                                                \
        }                                                               \
                                                                        \
        if (left_port < 1 || right_port > 4) {                          \
            PyErr_Format(PyExc_ValueError,                              \
                         "Left port must be 1-4, got: %d", left_port);  \
        }                                                               \
                                                                        \
        if (right_port < 1 || right_port > 4) {                         \
            PyErr_Format(PyExc_ValueError,                              \
                         "Right port must be 1-4, got: %d", right_port); \
        }                                                               \
                                                                        \
        if (validate_power(power)) {                                    \
            return NULL;                                                \
        }                                                               \
                                                                        \
        if (check_closed(self)) {                                       \
            return NULL;                                                \
        }                                                               \
                                                                        \
        if (NXT_ ## verb ## direction(&self->nxt,                       \
                                           time,                        \
                                           power,                       \
                                           (motor_port) left_port - 1,  \
                                           (motor_port) right_port - 1)) { \
            PyErr_SetString(PyExc_IOError,                              \
                            "Failed to " #verb " " #direction);         \
            return NULL;                                                \
        }                                                               \
                                                                        \
        Py_RETURN_NONE;                                                 \
    }

DRIVE_FN(drive, forward)
DRIVE_FN(drive, backward)
DRIVE_FN(turn, left)
DRIVE_FN(turn, right)

PyDoc_STRVAR(nxt_set_motor_doc,
             "Sets the power of a motor.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "port : int\n"
             "    The port of the motor to set the power of.\n"
             "power : int\n"
             "    The power to set the motor to: [-100, 100].\n"
             "\n"
             "Raises\n"
             "------\n"
             "ValueError\n"
             "    Raised if the port is out of bounds or the power is not\n"
             "    in the range [-100, 100].\n"
             "IOError\n"
             "    Raised when communication with the NXT fails.\n");

static PyObject*
nxt_set_motor(nxtobject *self, PyObject *args, PyObject *kwargs)
{
    char *keywords[] = {"port", "power", NULL};
    int port;
    int power;

    if (!PyArg_ParseTupleAndKeywords(args,
                                     kwargs,
                                     "ii",
                                     keywords,
                                     &port,
                                     &power)) {
        return NULL;
    }

    if (validate_port(port)) {
        return NULL;
    }

    if (validate_power(power)) {
        return NULL;
    }

    if (check_closed(self)) {
        return NULL;
    }

    if (NXT_setmotor(&self->nxt, (motor_port) port - 1, power)) {
        PyErr_Format(PyExc_IOError,
                     "Failed to set motor on port %d to %d",
                     port,
                     power);
        return NULL;
    }

    Py_RETURN_NONE;
}

PyDoc_STRVAR(nxt_stop_motor_doc,
             "Stop a motor.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "port : int\n"
             "    The port of the motor to stop.\n"
             "\n"
             "Raises\n"
             "------\n"
             "ValueError\n"
             "    Raised if the port is out of bounds\n"
             "IOError\n"
             "    Raised when communication with the NXT fails.\n");


static PyObject*
nxt_stop_motor(nxtobject *self, PyObject *args, PyObject *kwargs)
{
    char *keywords[] = {"port", NULL};
    int port;

    if (!PyArg_ParseTupleAndKeywords(args,
                                     kwargs,
                                     "i",
                                     keywords,
                                     &port)) {
        return NULL;
    }

    if (validate_port(port)) {
        return NULL;
    }

    if (check_closed(self)) {
        return NULL;
    }

    if (NXT_stopmotor(&self->nxt, (motor_port) port - 1)) {
        PyErr_Format(PyExc_IOError,
                     "Failed to stop motor on port %d",
                     port);
        return NULL;
    }

    Py_RETURN_NONE;
}

PyDoc_STRVAR(nxt_stop_all_motors_doc,
             "Stop all of the motors.\n"
             "\n"
             "Raises\n"
             "------\n"
             "IOError\n"
             "    Raised when communication with the NXT fails.\n");


static PyObject*
nxt_stop_all_motors(nxtobject *self, PyObject *_ __attribute__((unused)))
{
    if (check_closed(self)) {
        return NULL;
    }

    if (NXT_stopallmotors(&self->nxt)) {
        PyErr_SetString(PyExc_IOError, "Failed to stop all motors.");
        return NULL;
    }

    Py_RETURN_NONE;
}

PyDoc_STRVAR(nxt_close_doc,
             "Close the connection to the Lego NXT.\n");

static PyObject*
nxt_close(nxtobject *self, PyObject *_ __attribute__((unused)))
{
    if (self->closed) {
        Py_RETURN_NONE;
    }

    NXT_destroy(&self->nxt);
    self->closed = 1;
    Py_RETURN_NONE;
}

static PyObject*
nxt_enter(nxtobject *self, PyObject *_ __attribute__((unused)))
{
    if (check_closed(self)) {
        return NULL;
    }

    return (PyObject*) self;
}

PyDoc_STRVAR(nxt_get_battery_level_doc,
             "The charge remaining in mV.\n");

static PyObject*
nxt_get_battery_level(nxtobject *self, void *_ __attribute__((unused)))
{
    int battery_level;

    if (check_closed(self)) {
        return NULL;
    }

    if ((battery_level = NXT_battery_level(&self->nxt))) {
        PyErr_SetString(PyExc_IOError, "Failed to read the battery level");
        return NULL;
    }

    return PyLong_FromLong(battery_level);
}

PyDoc_STRVAR(nxt_dev_id_doc,
             "The device id of the connected lego NXT.\n");

static PyObject*
nxt_get_dev_id(nxtobject *self, void *_ __attribute__((unused)))
{
    if (check_closed(self)) {
        return NULL;
    }

    return PyLong_FromLong(self->nxt.dev_id);
}

static PyGetSetDef nxt_getsets[] = {
  {"battery_level",
   (getter) nxt_get_battery_level,
   NULL,
   nxt_get_battery_level_doc,
   NULL},
  {"dev_id",
   (getter) nxt_get_dev_id,
   NULL,
   nxt_dev_id_doc,
   NULL},
  {NULL},
};

PyDoc_STRVAR(nxt_closed_doc,
             "Is the connection to the Lego NXT closed?\n");

static PyMemberDef nxt_members[] = {
    {"closed", T_INT, offsetof(nxtobject, closed), READONLY, nxt_closed_doc},
    {NULL},
};

static PyMethodDef nxt_methods[] = {
    {"play_tone",
     (PyCFunction) nxt_play_tone,
     METH_VARARGS | METH_KEYWORDS,
     nxt_play_tone_doc},
    {"stay_alive",
     (PyCFunction) nxt_stay_alive,
     METH_NOARGS,
     nxt_stay_alive_doc},
    {"init_button",
     (PyCFunction) nxt_init_button,
     METH_VARARGS | METH_KEYWORDS,
     nxt_init_button_doc},
    {"init_light",
     (PyCFunction) nxt_init_light,
     METH_VARARGS | METH_KEYWORDS,
     nxt_init_light_doc},
    {"is_pressed",
     (PyCFunction) nxt_is_pressed,
     METH_VARARGS | METH_KEYWORDS,
     nxt_is_pressed_doc},
    {"read_light",
     (PyCFunction) nxt_read_light,
     METH_VARARGS | METH_KEYWORDS,
     nxt_read_light_doc},
    {"drive_forward",
     (PyCFunction) nxt_drive_forward,
     METH_VARARGS | METH_KEYWORDS,
     nxt_drive_forward_doc},
    {"drive_backward",
     (PyCFunction) nxt_drive_backward,
     METH_VARARGS | METH_KEYWORDS,
     nxt_drive_backward_doc},
    {"turn_left",
     (PyCFunction) nxt_turn_left,
     METH_VARARGS | METH_KEYWORDS,
     nxt_turn_left_doc},
    {"turn_right",
     (PyCFunction) nxt_turn_right,
     METH_VARARGS | METH_KEYWORDS,
     nxt_turn_right_doc},
    {"set_motor",
     (PyCFunction) nxt_set_motor,
     METH_VARARGS | METH_KEYWORDS,
     nxt_set_motor_doc},
    {"stop_motor",
     (PyCFunction) nxt_stop_motor,
     METH_VARARGS | METH_KEYWORDS,
     nxt_stop_motor_doc},
    {"stop_all_motors",
     (PyCFunction) nxt_stop_all_motors,
     METH_NOARGS,
     nxt_stop_all_motors_doc},
    {"close",
     (PyCFunction) nxt_close,
     METH_NOARGS,
     nxt_close_doc},
    {"__enter__",
     (PyCFunction) nxt_enter,
     METH_NOARGS,
     NULL},
    {"__exit__",
     (PyCFunction) nxt_close,
     METH_VARARGS,
     NULL},
    {NULL},
};

PyDoc_STRVAR(nxt_doc,
             "An object that represents a connection to a Lego NXT.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "mac_address : str\n"
             "    The mac address of the nxt robot.\n");

static PyTypeObject nxt_type = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
    "pynxt.NXT",                                /* tp_name */
    sizeof(nxtobject),                          /* tp_basicsize */
    0,                                          /* tp_itemsize */
    (destructor) nxt_dealloc,                   /* tp_dealloc */
    0,                                          /* tp_print */
    0,                                          /* tp_getattr */
    0,                                          /* tp_setattr */
    0,                                          /* tp_reserved */
    (reprfunc) nxt_repr,                        /* tp_repr */
    0,                                          /* tp_as_number */
    0,                                          /* tp_as_sequence */
    0,                                          /* tp_as_mapping */
    0,                                          /* tp_hash */
    0,                                          /* tp_call */
    (reprfunc) nxt_repr,                        /* tp_str */
    PyObject_GenericGetAttr,                    /* tp_getattro */
    0,                                          /* tp_setattro */
    0,                                          /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                         /* tp_flags */
    nxt_doc,                                    /* tp_doc */
    0,                                          /* tp_traverse */
    0,                                          /* tp_clear */
    0,                                          /* tp_richcompare */
    0,                                          /* tp_weaklistoffset */
    0,                                          /* tp_iter */
    0,                                          /* tp_iternext */
    nxt_methods,                                /* tp_methods */
    nxt_members,                                /* tp_members */
    nxt_getsets,                                /* tp_getset */
    0,                                          /* tp_base */
    0,                                          /* tp_dict */
    0,                                          /* tp_descr_get */
    0,                                          /* tp_descr_set */
    0,                                          /* tp_dictoffset */
    0,                                          /* tp_init */
    0,                                          /* tp_alloc */
    nxt_new,                                    /* tp_new */
};

#define MODULE_NAME "pynxt._nxt"
PyDoc_STRVAR(module_doc,
             "Bluetooth control for the Lego NXT.");

#if !COMPILING_IN_PY2
static struct PyModuleDef _nxt_module = {
    PyModuleDef_HEAD_INIT,
    MODULE_NAME,
    module_doc,
    -1,
};
#endif  /* !COMPILING_IN_PY2 */

PyMODINIT_FUNC
#if !COMPILING_IN_PY2
#define ERROR_RETURN NULL
PyInit__nxt(void)
#else
#define ERROR_RETURN
init_nxt(void)
#endif  /* !COMPILING_IN_PY2 */
{
    PyObject *m;

    if (PyType_Ready(&nxt_type)) {
        return ERROR_RETURN;
    }

#if !COMPILING_IN_PY2
    if (!(m = PyModule_Create(&_nxt_module)))
#else
    if (!(m = Py_InitModule3(MODULE_NAME, NULL, module_doc)))
#endif  /* !COMPILING_IN_PY2 */
    {
        return ERROR_RETURN;
    }

    if (PyModule_AddObject(m, "NXT", (PyObject*) &nxt_type)) {
        Py_DECREF(m);
        return ERROR_RETURN;
    }

#if !COMPILING_IN_PY2
    return m;
#endif  /* !COMPILING_IN_PY2 */
}
