import os
import yaml

from _suggestr import *

def open_dictionary(path):
    path = os.path.expanduser(path)
    fs = os.path.getsize(path)
    return load_dictionary(path, 0, fs, 0)

def get_with_context(context, D, sess, key_layout):
    if context[-1].isspace():
        prev_words = [PrevWordInfo(pw.strip(), False) for pw in context.split()]
        x = get_suggestion("", prev_words[::-1], D, sess, key_layout)
    else:
        s = context.split()
        prev_words = [PrevWordInfo(pw, False) for pw in s[:-1]] if len(s) else []
        x = get_suggestion(s[-1], prev_words[::-1], D, sess, key_layout)
    return x

def layout_to_keys(fn):
    with open(fn) as fi:
        keyboard = yaml.safe_load(fi.read())

    total_size = keyboard['bounds']

    base_kb = keyboard['views']['base']

    outlines = keyboard['outlines']
    default_ol = outlines['default']
    line_height = default_ol['bounds']['height']

    keys = {}
    key_list = []
    y_pos = 0
    for line in base_kb:
        ordered_chars = line.split()

        x_pos = 0
        for char in ordered_chars:
            if char == ' ':
                contin
            c = char
            y = y_pos
            x = x_pos
            if char in keyboard['buttons']:
                btn_el = keyboard['buttons'][char]
                char_ol_id = btn_el['outline']
                oline = outlines[char_ol_id]
                width = oline['bounds']['width']
                height = oline['bounds']['height']
            else:
                width = default_ol['bounds']['width']
                height = default_ol['bounds']['height']

            x_pos += width

            keys[char] = (x, y, width, height)
            if char.isalnum() and len(char) == 1:
                key_list.append(Key(ord(char), int(x), int(y), int(width), int(height), 0, 0))

        y_pos += line_height

    return key_list