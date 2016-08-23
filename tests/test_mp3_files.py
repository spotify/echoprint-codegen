"""
Tests the echoprint-codegen running on somename.mp3 and matching
its result with a somename.json in the same path. The number of tests
is the number of MP3 files in this directory

Author: Danilo J. S. Bellini
Date: 2016-08-23
"""
import json, subprocess, shlex, io, os, pytest

test_dir = os.path.dirname(__file__)
mp3_files = [name for name in os.listdir(test_dir)
             if name.lower().endswith("mp3")]

def get_echoprint_output(input_file):
    process = subprocess.Popen(["./echoprint-codegen", "-h", input_file],
                               stdout = subprocess.PIPE)
    return process.communicate()[0]

def to_json(data):
    return json.loads(data, encoding="utf-8")

@pytest.mark.parametrize("mp3_fname_no_path", mp3_files)
def test_one_song(mp3_fname_no_path):
    raw_fname = os.path.splitext(mp3_fname_no_path)[0]
    json_fname_no_path = os.path.extsep.join([raw_fname, "json"])

    json_fname = os.path.relpath(os.path.join(test_dir, json_fname_no_path))
    mp3_fname = os.path.relpath(os.path.join(test_dir, mp3_fname_no_path))

    with io.open(json_fname, "r", encoding="utf-8") as f:    
        expected = f.read()
    output = get_echoprint_output(mp3_fname).decode("utf-8")

    json_expected = to_json(expected)
    json_output = to_json(output)

    # Some keys that might change, so there's no need to compare
    for key in ["version", "codegen_time", "decode_time"]:
        del json_expected["metadata"][key]
        del json_output["metadata"][key] # This actually asserts if the key
                                         # exists in the output

    # "Decompose" the dictionaries when testing, effectively destroying them,
    # but with a more helpful fail message when something differs
    for key in json_expected["metadata"].keys():
        print("metadata: " + key)
        assert json_expected["metadata"][key] == json_output["metadata"][key]
        del json_expected["metadata"][key]
        del json_output["metadata"][key]
    for key in json_expected.keys():
        assert json_expected[key] == json_output[key]
        del json_expected[key]
        del json_output[key]
    assert json_output == json_expected == {}
