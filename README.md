# xml-parser

A super basic XML parser.  Built it to make sure the base project works but also for fun.

Tokenizes the XML document into a series of tokens.
It is trivial to take this stream and turn it into a tree.

## Building

1. Clone the repository and the submodules.

```
git clone https://github.com/czipperz/xml-parser
cd xml-parser
git submodule init
git submodule update
```

2. Build xml-parser by running (on all platforms):

```
./build-release
```

3. After building, xml-parser can be ran via `./build/release/xml-parser`.

## Optimizing
We use Tracy to optimize xml-parser.  See the
[manual](https://bitbucket.com/wolfpld/tracy/downloads/tracy.pdf) for more information.

To prepare we have to build xml-parser with Tracy enabled and also build Tracy's
profiler.  Once both are built, we then run the profiler and xml-parser at the same time.

Build xml-parser with Tracy enabled:
```
./build-tracy
```

Build `tracy/profiler` by following the instructions in the Tracy manual.  On *nix:
```
cd tracy/profiler/build/unix
make release
```

Then we run Tracy:
```
./tracy/profiler/build/unix/Tracy-release
```

Then run xml-parser with Tracy enabled.  Run it as the
super user to enable context switching recognition.
```
sudo ./build/tracy/xml-parser
```
