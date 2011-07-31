all: client.py _client.so

client.py: client.swig client.h
	swig -c++ -python client.swig

_client.so: client.py setup.py client.cpp
	python setup.py build_ext --inplace

clean:
	-rm -f -r *.o *.so client.py client.pyc client_wrap.cxx build
