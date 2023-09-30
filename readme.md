# Appy's Master Copy

## Build (normal way)
```
cd src
make && ./sim <file>
```

## Build (recommended way)
From, root directory, run:
```
./try.sh <test file>  
```
You can find the test file names in the test directory.


## Testing 
```
cd src
make test && ./test
```

Please refer to the main function in `shell.c`. The target `test` compiles with the `TEST_MODE` flag, which rewires main logic.
