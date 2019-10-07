# Inverted index for set storage

## Supported operations

The inverted index supports set-containment queries, such as

- *Set existence* - checks if a set that is equal to a query set exists in the index.
- *Subset existence* - check if a subset of a query set exists in the index.
- *All subset retrieval* - retrieves all the subsets of a query set from the index.
- *Set existence* - check if a query set exists in the index.
- *Superset existence* - check if a superset of a query set exists in the index.
- *All superset retrieval* - retrieves all the supersets of a query set from the index.

## Compiling binaries

Run the following command in the *src* directory:

```bash
git clone https://github.com/nick-ak96/InvertedIndex.git
cd src
make
```

and you will get the **inverted_index** executable.

## Running the program

### Arguments

##### Input file

The program accepts an input file that is used for constructing an inverted index from the standard input. The input file must have the following format:

- Each input set corresponds to a single line
- Each element of the set is an integer
- Each element is delimited with a coma ','

##### Test file

The file with test query sets must be provided as the first argument. The test file must have the same format as an input file (described above).

##### Experiment mode

The second argument is the test mode, which can be either **sb** (subset existence query), **asb** (all subsets retrival query), **eq** (equality existence query), **sp** (superset existence query) or **asp** (all supersets retrival query).

##### Result file

The result of the tests is outputed to the standard output, so it can be easily redirected to a file.

##### Example of running the program:

```bash
./inverted_index < my_input_file my_test_file sb > my_result_file
```

### Result format

The output of the program is constructed as follows. Each test query corresponds to a line in the result ouput. Each line contains:

- A query set (elements of a set are delimited with a coma ',');
- A result of: 
	- sb, eq, sp queries: val=true/false;
	- asb, asp queries: sets=number\_of\_sets;
- Time taken in nanoseconds.
- A semicolon ';' that closes the record.

Lines are delimited with system specific line ending character.