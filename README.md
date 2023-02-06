# Office-Hours-Simulation

A program written in C to simulate a professor's office hours throughout the week using different priorities.

## Author
Dakota Kallas

## Usage
The program takes in two command-line arguments. The first indicates what algorithm to use for the simulation and the second is the name of the file for the simulation. It is assumed that the file exists in the current directory, or the relative or full path is provided. To indicate that FIFO is to be used, then `-f` is given as the first command-line argument. To indicate that advising appointments are prioritized, `-a` is used. To indicate that project demos are prioritized, `-d` is used. To indicate that project questions are prioritized, `-q` is used.

## Input File
Example input files have been provided in the forms of `infile1.txt`, `infile2.txt`, and `infile3.txt`.
The input file has the following components:
- Name of the professor (char *)
- Minutes of Office Hours for Monday (int)
- Minutes of Office Hours for Tuesday (int)
- Minutes of Office Hours for Wednesday (int)
- Minutes of Office Hours for Thursday (int)
- Minutes of Office Hours for Friday (int)
- Number of students (int)
- For each student, the following is given:
  - Name of the student (char *)
  - Type of visit (Q for question, D for demo, A for advising)
  - Visit length in minutes (int)
  - Days the student is available (one character per day, with R representing Thursday)

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.
