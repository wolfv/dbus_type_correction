# suggestr

suggestr is a Python library for efficient typers -- those who type fast and 
make mistakes.
The library is basically the ripped out bits and pieces from the (old) Android
Open Source keyboard text correction, and then a Pyton interface to that. 

Right now it allows you to load a dictionary in the correct format, and then 
pass some context and some input, and the text correction will try to match 
words to the provided input. 

Keyboard layouts can be read from the yaml files in `layouts`. They come from the
Librem5 project for squeekboard, their software keyboard implementation.
It would be interesting to also have some layouts for specific notebook models in 
the future.

For example:

```py
import suggestr
dct, sess = suggestr.open_dictionary("./dicts/main_en.dict")
key_layout = suggestr.layout_to_keys('./layouts/german.yaml')

word = "teet"
context = ["this", "is", "a"]

print("Completion context: ", context)
print("current input: ", word)

sug = suggestr.get_suggestion(word, context, dct, sess, key_layout)
print("Suggestions: ", sug)
```

This will print suggestions such as:

```
['feet', 'test', 'rest', 'year', 'tree', 'teeth', 'deer', 'Trey', 'rear',
 'tear', 'reef', 'Reed', 'here', 'reed', 'grey', 'free', 'feed']
```

I hope that this library will prove helpful!

## Building

suggestr requires CMake, a C++ compiler, and Python with headers installed as well
as pybind11.

```
mkdir build
cd build
cmake ..
make
python3 python/test.py # to run some example code
```

