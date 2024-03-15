
# From Nand To Tetris : Building a Modern Computer from First Principles

  
This repo contains my implementations of the Nand2Tetris course' projects on Coursera. The course consists of two parts :
- [Part I (Hardware)](https://www.coursera.org/learn/build-a-computer) : from Project 1 to Project 6
- [Part II (Software)](https://www.coursera.org/learn/nand2tetris2) : from Project 7 to Project 12

This is a brief overview of the realized projects :

* Project 1: **Designing Logic Gates** ---> Creating a description of elementary logic gates like AND, OR, XOR, Multiplexer, Demultiplexer... using a special hardware description language (HDL)

* Project 2: **ALU** ---> Constructing a full adder chip and Arithmetic Logic Unit (ALU) based on the previous logic gates.

* Project 3: **Registers & Program Counter** ---> Creating a set of registers using a D flip-flop and a bunch of RAM Units by accumulating registers.

* Project 4: **Programming with Assembly** ---> Writing two programs using the special Hack Assembly.

* Project 5: **Putting it all together** ---> Designing a CPU, a full memory, and putting it all together to form a fully functioning Hack computer.

* Project 6: **Assembler** ---> Implementing a Hack Assembler in C++ to translate Hack Assembly Language into binary machine language that conforms to the instruction set of the Hack CPU.

* Project 7 & 8: **VM Translator** ---> Implementing a translator that takes a VM code (Virtual Machine code) and translates it into Hack assembly code. This is the first step to implement a two-tier compiler (like Java's JVM that takes the bytecode and translates it into the machine language of the target platform).

* Project 9: **Programming with Jack Language** ---> Creating a naive Tik-Tak-Toe game using the Jack laguage.

* Project 10: **Jack Analyzer** ---> Implementing a syntax analyzer that produces XML code. The analyzer consists of two main modules : a Tokenizer and a Parser. The parser needs to have the ability to understand the source code and the syntactic structure of the Jack commands (It must understand the Jack grammar).

* Project 11: **Code Generator** ---> Rewriting certain elements from the implemented parser that are responsible for generating code to get a compiler that translates the high-level Jack code into VM code.

* Project 12: **Operating System** ---> Developing a basic operating system that is packaged in a set of class libraries written in Jack language (following the same manner legacy OSs were created) using a bootstrapping strategy


All the programming assignments are written in C++.

Feel free to comment on the code and to report any issues.

<br/>
<br/>



## About Nand2Tetris

  
#### Part I :
In this project-centered course* you will build a modern computer system, from the ground up. We’ll divide this fascinating journey into six hands-on projects that will take you from constructing elementary logic gates all the way through creating a fully functioning general purpose computer. In the process, you will learn - in the most direct and constructive way - how computers work, and how they are designed.

The course consists of six modules, each comprising a series of video lectures, and a project. You will need about 2-3 hours to watch each module's lectures, and about 5-10 hours to complete each one of the six projects. The course can be completed in six weeks, but you are welcome to take it at your own pace. You can watch a TED talk about this course by Googling "nand2tetris TED talk".

<br/>

#### Part II :

In this project-centered course you will build a modern software hierarchy, designed to enable the translation and execution of object-based, high-level languages on a bare-bone computer hardware platform. In particular, you will implement a virtual machine and a compiler for a simple, Java-like programming language, and you will develop a basic operating system that closes gaps between the high-level language and the underlying hardware platform. In the process, you will gain a deep, hands-on understanding of numerous topics in applied computer science, e.g. stack processing, parsing, code generation, and classical algorithms and data structures for memory management, vector graphics, input-output handling, and various other topics that lie at the very core of every modern computer system.

This is a self-contained course: all the knowledge necessary to succeed in the course and build the various systems will be given as part of the learning experience. The only prerequisite is knowledge of programming at the level acquired in introduction to computer science courses. All the software tools and materials that are necessary to complete the course will be supplied freely after you enrol in the course. This course is accompanied by the textbook "The Elements of Computing Systems" (Nisan and Schocken, MIT Press). While not required for taking the course, the book provides a convenient coverage of all the course topics. The book is available in either hardcopy or ebook form, and MIT Press is offering a 30% discount off the cover price by using the discount code MNTT30 at https://mitpress.mit.edu/books/elements-computing-systems. The course consists of six modules, each comprising a series of video lectures, and a project. You will need about 2-3 hours to watch each module's lectures, and about 15 hours to complete each one of the six projects. The course can be completed in six weeks, but you are welcome to take it at your own pace. You can watch a TED talk about this course by Googling "nand2tetris TED talk".

 *About Project-Centered Courses: Project-centered courses are designed to help you complete a personally meaningful real-world project, with your instructor and a community of learners with similar goals providing guidance and suggestions along the way. By actively applying new concepts as you learn, you’ll master the course content more efficiently; you’ll also get a head start on using the skills you gain to make positive changes in your life and career. When you complete the course, you’ll have a finished project that you’ll be proud to use and share.

### Course official website 

[Home | nand2tetris](https://www.nand2tetris.org/)


<br/>
<br/>

												        AKO 2022
