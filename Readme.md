# Philosophy

Pwnto-driver is a reimplementation of pwntools in C instead of python. The goal is to fix some pwntools issues. Pwnto-drivers will
 - provide ability to exploit drivers.
 - provide ability to create worm from imported pwnto-driver library.
 
As the dev have some experience with the old pwntools, the new tool aims to fix some known issues:
 - windows program and drivers support. More than pwntools.
 - Linux driver support (more than pwn++ that is written in C++ instead of C).
 
# I Build

# I.1 Install building dependencies

```
choco install --yes python --version 3.13.0-rc2
choco install --yes swig
choco install --yes llvm
```

# I.2 Build/Clean project

In order to build pwnto-driver including swig bindings, you must specify your python path. For example, if you installed python 3 with the command `choco install --yes python --version 3.13.0-rc2`, the you could build with:

```shell
make all PYENV=C:\Python313
```
clean with

```shell
make clean
```

# II Contributing

# Install dev dependencies

```shell
choco install --yes doxygen.install
choco install --yes doxygen.portable
choco install --yes swig
```

# II.1 Bindings with swig

Each set of .h/.c of file you create MUST be followed by another .i file with the same name containing swig binding definition.

It will then autogenerate binding so that our c program with c perf and c syntax will be available by importation in most of the languages including python.

Watch the swig doc for more informations.

# II.2 Documentating with doxygen

# II.3 Testing with integrated doxygen doctest

# alternatives:

 - [pwntools](https://github.com/Gallopsled/pwntools) : Works only on Linux. Not windows. No ability to upload in drivers.
 - [pwn++](https://github.com/hugsy/pwn--) : Does not provide native C support. Then no ability to upload in drivers. 