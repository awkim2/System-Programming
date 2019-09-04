Homework 0
----------

### Watch the videos and write up your answers to the following questions

**Important!**

The virtual machine-in-your-browser and the videos you need for HW0 are here:

<http://cs-education.github.io/sys/>

Questions? Comments? Use Piazza: <https://piazza.com/illinois/fall2019/cs241>

The in-browser virtual machine runs entirely in Javascript and is fastest in Chrome. Note the VM and any code you write is reset when you reload the page, **so copy your code to a separate document.** The post-video challenges are not part of homework 0 but you learn the most by doing rather than just passively watching - so we suggest you have some fun with each end-of-video challenge.

HW0 questions are below. Please use this document to write the answers. This will be hand graded.

### Chapter 1

In which our intrepid hero battles standard out, standard error, file descriptors and writing to files

1.  **Hello, World! (system call style)** Write a program that uses `write()` to print out “Hi! My name is &lt;Your Name&gt;”.

```c
// Your code here
int main() {  
    write(1,"Hi! My name is <Your Name>", 26);  
	return 0;  
}  
```




2.  **Hello, Standard Error Stream!** Write a function to print out a triangle of height `n` to standard error. Your function should have the signature `void write_triangle(int n)` and should use `write()`. The triangle should look like this, for n = 3:

```
*
**
***
```

```c
// Your code here
void write_triangle(int n){  
	int i,j;  
	for(i = 0; i < n; i++){  
		for(j = 0; j <= i; j++){  
	        write(2, "*",1);  
	        }  
	write(2,"\n",1);  
	}  
}  
int main() {  
	write_triangle(3);  
	return 0;  
}  
```



3.  **Writing to files** Take your program from “Hello, World!” modify it write to a file called `hello_world.txt`. Make sure to to use correct flags and a correct mode for `open()` (`man 2 open` is your friend).

```c
// Your code here
int main() {  
	mode_t mode = S_IRUSR |S_IWUSR;  
	int file_ = open("hello_world.txt",O_CREAT|O_TRUNC| O_RDWR, mode);  
	write(file_, "Hi! My name is <Your Name>", 26);  
	return 0;     
}  
```



5. **Not everything is a system call** Take your program from “Writing to files” and replace `write()` with `printf()`. *Make sure to print to the file instead of standard out!*

```c
// Your code here
int main() {  
	close(1);  
	mode_t mode = S_IRUSR |S_IWUSR;  
	int file_ = open("hello_world.txt",O_CREAT|O_TRUNC| O_RDWR, mode);  
	printf("Hi! My name is <Your Name>\n");  
	close(file_);  
	return 0;     
}
```



6.  What are some differences between `write()` and `printf()`?

```c
// Your code here
The write() is a system called implemented by the operating system, and run in kernel mode. However, printf() is a function call in library which are implemented in user mode and printf() will eventually invoke write().
```


### Chapter 2

Sizing up C types and their limits, `int` and `char` arrays, and incrementing pointers

1.  How many bits are there in a byte?

```c
// Your answer here
At least 8 bits
```


2.  How many bytes are there in a `char`?

```c
// Your answer here
1 byte.
```


3.  How many bytes the following are on your machine? 

* `int`: 4
* `double`: 8
* `float`: 4
* `long`: 4
* `long long`: 8

4.  On a machine with 8 byte integers, the declaration for the variable `data` is `int data[8]`. If the address of data is `0x7fbd9d40`, then what is the address of `data+2`?

```c
// Your answer here
0x7fbd9d50
```


5.  What is `data[3]` equivalent to in C? Hint: what does C convert `data[3]` to before dereferencing the address? Remember, the type of a string constant `abc` is an array.

```c
// Your answer here
 data+3 or 3[data]
```


6.  Why does this segfault?

```c
char *ptr = "hello";
*ptr = 'J';
Hello”is a read only constant literal string which cannot be changed
```

7.  What does `sizeof("Hello\0World")` return?

```c
// Your answer here
12
```


8.  What does `strlen("Hello\0World")` return?

```c
// Your answer here
5
```


9.  Give an example of X such that `sizeof(X)` is 3.

```c
// Your code here
sizeof("aa")
```

10. Give an example of Y such that `sizeof(Y)` might be 4 or 8 depending on the machine.

```c
// Your code here
int Y
sizeof(Y)
```

### Chapter 3

Program arguments, environment variables, and working with character arrays (strings)

1.  What are two ways to find the length of `argv`?
	Print the argc or using for loop to count the number of argv

2.  What does `argv[0]` represent?
	Program name
3.  Where are the pointers to environment variables stored (on the stack, the heap, somewhere else)?
	They are located somewhere else and we can use extern to use them
4.  On a machine where pointers are 8 bytes, and with the following code:

    ``` c
    char *ptr = "Hello";
    char array[] = "Hello";
    ```

    What are the values of `sizeof(ptr)` and `sizeof(array)`? Why?

```c
// Your answer here
	sizeof(ptr) == 8 and sizeof(array) ==6 because sizeof(ptr) actually the number of bytes to hold the char pointer which is 8 bytes and sizeof(array) is the size of total length to hold the array.
```

5.  What data structure manages the lifetime of automatic variables?
	Stack

### Chapter 4

Heap and stack memory, and working with structs

1.  If I want to use data after the lifetime of the function it was created in ends, where should I put it? How do I put it there?
	Using keyword static in front of a variable or using malloc to allocate some heap memory are two feasible solutions If you want to use data after the lifetime of the function it was created in ends.
2.  What are the differences between heap and stack memory?
	Stack memory will free automatically after the function return, but heap memory will not free automatically and it will last until the end of program process or we manually free the memory.
3.  Are there other kinds of memory in a process?
	Kernel memory, Text segment, Data segment 
4.  Fill in the blank: “In a good C program, for every malloc, there is a \_\_\_”.
	"In a good C program, for every malloc, there is a free".
5.  What is one reason `malloc` can fail?
	Heap memory are full and cannot allocate new space.
6.  What are some differences between `time()` and `ctime()`?
	Time() is a system call which will return a time_t pointer, and ctime() is a function call in time library which will return a readable format of current time.
7.  What is wrong with this code snippet?

``` c
free(ptr);
free(ptr);
```
	Double free the same memory

8.  What is wrong with this code snippet?

``` c
free(ptr);
printf("%s\n", ptr);
```
	Access some memory which already do not exist, and ptr becomes a dangling pointer

9.  How can one avoid the previous two mistakes?
	Only free memory once and after free the memory, assign the pointer to NULL

10. Use the following space for the next four questions

```c
// 10

// 12

// 13

	struct Person {  
	    char* name;  
	    int age;  
	    struct Person** list;  
	};  
	typedef struct Person Person;  
	  
	Person* create(const char* name, const int age){  
	    Person* p = (Person*)malloc(sizeof(Person));  
	    p->name = strdup(name);  
	    p->age = age;  
	    p->list = malloc(sizeof(Person*) * 10);  
	    int i;  
	    for (i=0; i<10; i++) {  
	        p->list[i] = NULL;  
	    }  
	    return p;  
	}  
	void destory(Person* p){  
	    free(p->list); p->list = NULL;  
	    free(p->name); p->name = NULL;  
	    free(p); p = NULL;  
	}  
	  
	int main() {  
	    Person* as = create("Agent Smith", 128);  
	    Person* sm = create("Sonny Moore", 256);  
	    as->list[0] = sm;  
	    sm->list[0] = as;  
	    destory(as);  
	    destory(sm);  
	    return 0;  
	}

```

* Create a `struct` that represents a `Person`. Then make a `typedef`, so that `struct Person` can be replaced with a single word. A person should contain the following information: their name (a string), their age (an integer), and a list of their friends (stored as a pointer to an array of pointers to `Person`s). 

*  Now, make two persons on the heap, “Agent Smith” and “Sonny Moore”, who are 128 and 256 years old respectively and are friends with each other. Create functions to create and destroy a Person (Person’s and their names should live on the heap).

* `create()` should take a name and age. The name should be copied onto the heap. Use malloc to reserve sufficient memory for everyone having up to ten friends. Be sure initialize all fields (why?).

* `destroy()` should free up not only the memory of the person struct, but also free all of its attributes that are stored on the heap. Destroying one person should not destroy any others.


### Chapter 5

Text input and output and parsing using `getchar`, `gets`, and `getline`.

1.  What functions can be used for getting characters from `stdin` and writing them to `stdout`?

2.  Name one issue with `gets()`.

3.  Write code that parses the string “Hello 5 World” and initializes 3 variables to “Hello”, 5, and “World”.

```c
// Your code here
```

4.  What does one need to define before including `getline()`?

5.  Write a C program to print out the content of a file line-by-line using `getline()`.

```c
// Your code here
```

### C Development

These are general tips for compiling and developing using a compiler and git. Some web searches will be useful here


1.  What compiler flag is used to generate a debug build?


2.  You fix a problem in the Makefile and type `make` again. Explain why this may be insufficient to generate a new build.


3.  Are tabs or spaces used to indent the commands after the rule in a Makefile?


4.  What does `git commit` do? What’s a `sha` in the context of git?


5.  What does `git log` show you?


6.  What does `git status` tell you and how would the contents of `.gitignore` change its output?


7.  What does `git push` do? Why is it not just sufficient to commit with `git commit -m ’fixed all bugs’ `?


8.  What does a non-fast-forward error `git push` reject mean? What is the most common way of dealing with this?


### Optional: Just for fun

-   Convert your a song lyrics into System Programming and C code covered in this wiki book and share on Piazza.

-   Find, in your opinion, the best and worst C code on the web and post the link to Piazza.

-   Write a short C program with a deliberate subtle C bug and post it on Piazza to see if others can spot your bug.

-   Do you have any cool/disastrous system programming bugs you’ve heard about? Feel free to share with your peers and the course staff on piazza.
