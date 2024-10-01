## Goals of Optimized File

- **Universality**
- **Compressibility**
- **No External Libraries**
- **Scalability**
- **Fast Read/Write Speed**

## Clone the Repository

First, clone the repository:

```
cd /path/to/your_working_directory
git clone https://github.com/forhadnmsu/Universal.git
```

In the ```setup.sh``` script use the below line if you would like to run the code in Rivanna machine: 

```source /project/ptgroup/spinquest/this-e1039.sh```
```#source /exp/seaquest/app/software/osg/users/$USER/e1039/core/this-e1039.sh```

## Generating the Toy Data

The Python script below generates toy events, which are primarily used for testing the file structure in the initial phase. Later, real data can be used for implementations. The output ROOT file will be created in the `/gen` folder and can serve as input for the structure-based format. To generate the toy data, simply run the following Python script:

```
source ../setup.sh
python3 EventGen.py
```

## Compilation of Files in the `Universal/` Directory
The following commands set up the shell environment and compile the code:

```
cd StructWrite (or VectorWrite) 
source setup.sh
cmake-this
make-this
```

## Finally, run the command below to create the structure-based ROOT file

```
cd StructWrite
./StructWrite
```

## And, run the command below to create the linear-vector-based ROOT file

```
cd VectorWrite
./VectorWrite
```

