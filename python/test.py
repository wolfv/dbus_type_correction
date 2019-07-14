import os
import suggestr

def open_dictionary(path):
    path = os.path.expanduser(path)
    fs = os.path.getsize(path)
    return suggestr.load_dictionary(path, 0, fs, 0)


if __name__ == '__main__':
    dct, sess = open_dictionary("../dicts/main_en.dict")
    word = "teet"
    context = ["this", "is", "a"]

    print("Completion context: ", context)
    print("current input: ", word)

    sug = suggestr.get_suggestion(word, context, dct, sess)
    print("Suggestions: ", sug)
