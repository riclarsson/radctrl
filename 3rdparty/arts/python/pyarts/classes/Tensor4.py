import ctypes as c
from collections.abc import Sized
from pyarts.workspace.api import arts_api as lib

from pyarts.classes.io import correct_save_arguments, correct_read_arguments
from pyarts.classes.ArrayBase import array_base

import numpy as np
from copy import deepcopy as copy


class Tensor4:
    """ ARTS Tensor4 data

    Properties:
        dims:
            Dimensionality of Tensor4 (constexpr Index)

        data:
            The data (numpy-array)

        size:
            Shape of the Tensor4 (tuple)

    """
    def __init__(self, data=None):
        if isinstance(data, c.c_void_p):
            self.__delete__ = False
            self.__data__ = data
        else:
            self.__delete__ = True
            self.__data__ = c.c_void_p(lib.createTensor4())
            if data is not None:
                self.data = data

    @property
    def dims(self):
        return 4

    @property
    def flat(self):
        return self.data.flat

    @property
    def data(self):
        """ The data (numpy-array) """
        if not np.any(self.shape == np.zeros(shape=(self.dims))):
            return np.ctypeslib.as_array(lib.getDataTensor4(self.__data__), self.shape)
        else:
            return np.ndarray(self.shape, buffer=np.array([]))

    @data.setter
    def data(self, val):
        x = copy(np.array(val))
        self.shape = x.shape
        self.data.flat[:] = x.flat[:]

    @property
    def shape(self):
        """ Shape of the Tensor4 (tuple) """
        return lib.booksTensor4(self.__data__), lib.pagesTensor4(self.__data__), lib.rowsTensor4(self.__data__), lib.colsTensor4(self.__data__),

    @shape.setter
    def shape(self, shape):
        if len(shape) == self.dims:
            lib.resizeTensor4(*shape, self.__data__)
        elif len(shape) < self.dims:
            shape = list(shape)
            while len(shape) < self.dims:
                shape.insert(0, 1)
            self.shape = shape
        else:
            raise RuntimeError("Cannot resize to larger dimensions")

    def print(self):
        """ Print to cout the ARTS representation of the class """
        lib.printTensor4(self.__data__)

    def __del__(self):
        if self.__delete__:
            lib.deleteTensor4(self.__data__)

    def __repr__(self):
        return repr(self.data)

    def set(self, other):
        """ Sets this class according to another python instance of itself """
        if isinstance(other, Tensor4):
            self.data = other.data
        else:
            self.data = other

    @staticmethod
    def name():
        return "Tensor4"

    def readxml(self, file):
        """ Reads the XML file

        Input:
            file:
                Filename to valid class-file (str)
        """
        if lib.xmlreadTensor4(self.__data__, correct_read_arguments(file)):
            raise OSError("Cannot read {}".format(file))

    def savexml(self, file, type="ascii", clobber=True):
        """ Saves the class to XML file

        Input:
            file:
                Filename to writable file (str)

            type:
                Filetype (str)

            clobber:
                Allow clobbering files? (any boolean)
        """
        if lib.xmlsaveTensor4(self.__data__, *correct_save_arguments(file, type, clobber)):
            raise OSError("Cannot save {}".format(file))
    
    def netcdfify(self):
        """ Create the NETCDF4 information required for writing this data
        
        Output: list that can be processed by netcdf.py, False arraytype
        """
        return [["data", self.data, float, {"nbooks": self.shape[0], "npages": self.shape[1], "nrows": self.shape[2], "ncols": self.shape[3]}]], False
    
    def denetcdf(self, group):
        """ Sets this based on a netcdf group
        
        Input:
            Group of data that can be interpreted as this's information
        """
        self.data = np.array(group.variables["data"])

    def __eq__(self, other):
        if isinstance(other, Tensor4):
            return (np.logical_or(self.data == other.data, np.logical_and(np.isnan(self.data), np.isnan(other.data)))).all()
        else:
            return self.data == other

    def __lt__(self, other):
        if isinstance(other, Tensor4):
            return (self.data < other.data).all()
        else:
            return self.data < other

    def __bool__(self):
        return bool((self.data != 0).any())

    def __array__(self):
        return self.data

    def __iadd__(self, val):
        self.data += np.array(val)
        return self

    def __isub__(self, val):
        self.data -= np.array(val)
        return self

    def __imul__(self, val):
        self.data *= np.array(val)
        return self

    def __imatmul__(self, val):
        self.data = self.data @ np.array(val)
        return self

    def __itruediv__(self, val):
        self.data /= np.array(val)
        return self

    def __ipow__(self, val):
        self.data **= np.array(val)
        return self

    def __add__(self, val):
        return Tensor4(self.data + np.array(val))

    def __sub__(self, val):
        return Tensor4(self.data - np.array(val))

    def __mul__(self, val):
        return Tensor4(self.data * np.array(val))

    def __matmul__(self, val):
        return self.data @ np.array(val)

    def __truediv__(self, val):
        return Tensor4(self.data / np.array(val))

    def __pow__(self, val):
        return Tensor4(self.data ** np.array(val))

    def __radd__(self, val):
        return Tensor4(np.array(val) + self.data)

    def __rsub__(self, val):
        return Tensor4(np.array(val) - self.data)

    def __rmul__(self, val):
        return Tensor4(np.array(val) * self.data)

    def __rmatmul__(self, val):
        return np.array(val) @ self.data

    def __rtruediv__(self, val):
        return Tensor4(np.array(val) / self.data)

    def __rpow__(self, val):
        return Tensor4(np.array(val) ** self.data)


# ArrayOfTensor4
exec(array_base(Tensor4))


# ArrayOfArrayOfTensor4
# exec(array_base(ArrayOfTensor4))


lib.createTensor4.restype = c.c_void_p
lib.createTensor4.argtypes = []

lib.deleteTensor4.restype = None
lib.deleteTensor4.argtypes = [c.c_void_p]

lib.printTensor4.restype = None
lib.printTensor4.argtypes = [c.c_void_p]

lib.xmlreadTensor4.restype = c.c_long
lib.xmlreadTensor4.argtypes = [c.c_void_p, c.c_char_p]

lib.xmlsaveTensor4.restype = c.c_long
lib.xmlsaveTensor4.argtypes = [c.c_void_p, c.c_char_p, c.c_long, c.c_long]

lib.colsTensor4.restype = c.c_long
lib.colsTensor4.argtypes = [c.c_void_p]

lib.rowsTensor4.restype = c.c_long
lib.rowsTensor4.argtypes = [c.c_void_p]

lib.pagesTensor4.restype = c.c_long
lib.pagesTensor4.argtypes = [c.c_void_p]

lib.booksTensor4.restype = c.c_long
lib.booksTensor4.argtypes = [c.c_void_p]

lib.resizeTensor4.restype = None
lib.resizeTensor4.argtypes = [c.c_long, c.c_long, c.c_long, c.c_long, c.c_void_p]

lib.getDataTensor4.restype = c.POINTER(c.c_double)
lib.getDataTensor4.argtypes = [c.c_void_p]
