# cmn-CommonObjects
Library with common objects for multiple projects

## Getting Started


### Prerequisites

OpenCV 3.x+<br/>
Boost 1.69+<br/>

### Installing

Windows
Run batch file in the configuration folder: configure\win\configure.bat<br/>
The batch file is used to configures the location of the third party libraries.<br/>

Run batch files from folder configure\win.<br/>
If new third party libraries are installed (i.e. new version of boost), run "configure\900. clear cache.bat". Please, note that all the files in build folders are deleted.<br/>

```
1. create_and_build_slns.bat<br/>
It creates the projects solution, and it builds the solutions and creates the binary files.<br/>
```
## List of projects

* enum<br/>
Classes to convert enumerators in string.<br/>

* io<br/>
Naive I/O classes for various objects.<br/>
RGBDRecorder requires the library StoreData.<br/>

* random<br/>
Random number generator<br/>

* sanity<br/>
Class to check the sanity of a file/directory structure<br/>

* shm_common<br/>
Classes with structures for shared memory.<br/>
SharedPCClient.hpp, SharedPCServer.hpp use shared_data_v2<br/>
SharedDataBase.hpp, SharedDataDerivedSample.hpp use shared_data_base (former SharedDataStructure.hpp and shared_data_v3).<br/>
The latest version of the shared memory is SharedDataBase and derived.<br/>

* string_common<br/>
Classes for common string operations.<br/>

## Running the tests

Windows Platform<br/>

Inside the sample folder there are examples for all the projects.<br/>

### Break down into end to end tests

TODO

### And coding style tests

TODO

## Contributing

All contributions are welcome :).

## Versioning

TODO

## Authors

* **Khoronus** - *Initial work* - [Khoronus](https://github.com/Khoronus)

## License

This project is licensed under (see the [LICENSE.md](LICENSE.md) file for details).

## Acknowledgments

* TODO
