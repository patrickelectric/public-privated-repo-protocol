#!/usr/bin/env python2

import json
import os
import re

# Force program to generate read and write in the same dir
__location__ = os.path.realpath(os.path.join(os.getcwd(), os.path.dirname(__file__))) + "/"

# Enable QByteArray constructor (disable for embedded use)
qbytearray_enabled = True

f = None

headerBytes = 8
checksumBytes = 2


def convert_short_type(t):

    # u/i
    ui = t[0]
    # number of bits
    nbits = t[1:]

    if ui == 'i':
        return 'int{0}_t'.format(nbits)

    if ui == 'u':
        return 'uint{0}_t'.format(nbits)


def get_type_string(t):

    # Remove vector info
    vector = False
    if t.find('[') != -1:
        t = t.split('[')[0]
        vector = True

    # Check for short type
    # This script will get X in u/iX (u8, i16)
    match = re.search('[u,i][0-9]{1,2}$', t)
    s = ''
    if match:
        s = convert_short_type(t)
    else:
        s = t

    # Append vector
    if vector:
        s = s + '*'

    return s

def get_cast_string(t):
    s = get_type_string(t)
    if s.find("*") != -1:
      s = "(" + s + ")"
    else:
      s = "*(" + s + "*)"

    return s

def get_type_base_size(t):

    if t.find('bool') != -1:
        return 1
    if t.find('int') != -1:
        return 4
    if t.find('float') != -1:
        return 4
    if t.find('double') != -1:
        return 8

    # Get short types
    # this regex will get the X in u/intX_t (uint8_t, int16_t)
    match = re.search('[0-9]{1,2}', t)
    if match:
        return int(int(match.group(0)) / 8)


def get_type_offset(t):

    o = get_type_base_size(t)

    first = t.find("[")
    if first != -1:
      last = t.find("]")
      o = o * int(t[first+1:last])

    return o

def file_write(f, data, debug = False):
    if debug:
        print(data),
    f.write(data)

def json_type_to_type_str(jtyp):
    return type_strings[jtyp]

def payload_size(payload_def):
    size = 0
    for field in payload_def:
        t = field["type"]
        size = size + get_type_offset(t)
    return size

if qbytearray_enabled == True:
    qba_constructor = "    ping_msg_{0}_{1}(QByteArray buf) : PingMessage(buf) {{}}"
else:
    qba_constructor = ""

def fmt_class(msg_group, msg_name, msg_def):

    msg_id = msg_def["id"]
    payload_def = msg_def["payload"]

    class_header = (
"""
class ping_msg_{0}_{1} : public PingMessage // ID {2}
{{
public:
""" + qba_constructor +
"""
    ping_msg_{0}_{1}(PingMessage& msg) : PingMessage(msg) {{}}
    ping_msg_{0}_{1}() {{
        msgData = std::vector<uint8_t>(8 + {3} + {4}, 0);
        msgData[0] = 'B';
        msgData[1] = 'R';
        (uint16_t&)msgData[2] = {3}; // payload size
        (uint16_t&)msgData[4] = {2}; // ID
        msgData[6] = 0;
        msgData[7] = 0;
    }}

""").format(msg_group, msg_name, msg_id, payload_size(payload_def), checksumBytes)

    file_write(f, class_header)

    get_strings = ""
    set_strings = ""
    current_offset = 0

    for field in payload_def:
        n = field["name"]
        t = field["type"]
      
        type_string = get_type_string(t)
        cast = get_cast_string(t)
        type_offset = get_type_offset(t)
        type_base_size = get_type_base_size(t)

        star = type_string.find("*") # is this field an array?

        # Output getter of this field
        f1 = ("%s")%(type_string)
        f2 = ("%s()")%(n)
        if star != -1: # this field is an array
            f3 = ("{ return (msgData.data() + 8 + %s); }") % (current_offset)
        else:
            f3 = ("{ %s d; memcpy(&d, (msgData.data() + 8 + %d), %d); return d; }") % (type_string, current_offset, type_base_size)

        get_string = ("    {:<10} {:<50} {:<20}\n").format(f1, f2, f3)

        # Output setter of this field
        f1 = ("void")
        if star != -1: # this field is an array
            f2 = ("set_%s_at(uint16_t i, %s %s)")%(n, type_string[:star], n)
            #f3 = ("{ *" + cast + " (msgData.data() + 8 + %d + i) = %s; }")%(current_offset, n)
            f3 = ("{ memcpy((msgData.data() + 8 + %d + i), &%s, %s); }")%(current_offset, n, type_base_size)
        else:
            f2 = ("set_%s(%s %s)")%(n, type_string, n)
            #f3 = ("{ " + cast + " (msgData.data() + 8 + %d) = %s; }")%(current_offset, n)
            f3 = ("{ memcpy((msgData.data() + 8 + %d), &%s, %s); }")%(current_offset, n, type_base_size)

        set_string = ("    {:<10} {:<50} {:<20}\n").format(f1, f2, f3)

        get_strings = get_strings + get_string
        set_strings = set_strings + set_string

        current_offset = current_offset + type_offset

    file_write(f, get_strings)
    file_write(f, set_strings)
    file_write(f, "};\n\n")

msg_definition_file = open(__location__ + "ping_protocol.json", "r")

msg_json = json.load(msg_definition_file)

msg_definition_file.close()

headers = []
for msg_group in msg_json.keys():
    headers.append("pingmessage_" + msg_group + ".h")
    f = open(__location__ + headers[-1], "w")
    file_write(f, "// Ping message c++ implementation\n")
    file_write(f, "// This file was originally generated by ping_generator.py\n\n")
    file_write(f, "#pragma once\n\n")
    file_write(f, '#include "pingmessage.h"\n')

    current_group = msg_json[msg_group]
    for msg_name in current_group.keys():
        msg_def = current_group[msg_name]
        fmt_class(msg_group, msg_name, msg_def)

    f.close()

f = open(__location__ + "pingmessage_all.h", "w")
file_write(f, "// Ping message c++ implementation\n")
file_write(f, "// This file was originally generated by ping_generator.py\n\n")
file_write(f, "#pragma once\n\n")
for include in headers:
    file_write(f, '#include "{}"\n'.format(include))

f.close()