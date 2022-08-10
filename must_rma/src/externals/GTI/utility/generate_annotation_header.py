# Helper script to generate the include header for user annotations

import argparse
from curses import meta
import os
from pydoc import cli

import xml.etree.ElementTree as ET

def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('-s', '--specs', metavar='file1 [files...]', type=str, nargs='+',
                        help='List of XML specifcation files to consider for the annotation header')
    parser.add_argument('-d', '--header-destination', metavar='path', type=str, default='./',
                        help='Destination path to generated header file')

    return parser.parse_args()


# Workaround for \ in f-strings
newline_char = '\n'
tab_char = '\t'


class HeaderGenerator():
    my_spec_files = []
    my_destination_file = 'GTI_Annotations.h'
    my_destination_path = '.'
    out = None
    my_enum_names = []
    call_id_offset = 4000
    

    def __init__(self, spec_files, destination_path):
        self.my_spec_files = spec_files
        self.my_destination_path = destination_path

        self.run()


    def run(self):
        with open(f'{self.my_destination_path}/{self.my_destination_file}', 'w') as self.out:
            for xml_spec in self.my_spec_files:
                self.process_xml_spec_file(xml_spec)

            self.postamble()


    def process_xml_spec_file(self, spec_file):
        tree = ET.parse(spec_file)
        root = tree.getroot()
        if not root.attrib['unique-name']:
            print(f"Error: could not find root tag for {spec_file}")
            return None

        for h in root.find('api-headers').findall('header'):
            self.out.write(f'#include "{h.text}"\n')
        self.out.write("\n")

        for f in root.find('functions').findall('function'):
            annotation_str = f.attrib.get('annotation')
            # Ignore non-annotation functions
            if not annotation_str or annotation_str == 'no':
                continue
            
            stripped_func_name = f.attrib['name']
            tmp_enum_name = f'ANNOTATE_{stripped_func_name}'.upper()
            self.my_enum_names.append(tmp_enum_name)
            
            # Parse annotation function arguments
            tmp_args = dict()
            for f_arg in f.find('function-arguments'):
                tmp_args[f_arg.get('name')] = f_arg.get('type')
                
            # Macro
            self.out.write(f'#define GTI_{stripped_func_name}({",".join(list(tmp_args.keys()))}) {{ \\\n'\
                f'{tab_char}{stripped_func_name}_args_t buf = {{{", ".join(list(tmp_args.keys()))}}}; \\\n'\
                f'\tannotation_data_t anndata = {{{tmp_enum_name}, &buf}}; \\\n'\
                '\tMPI_Pcontrol(4042, &anndata); \\\n'\
            '}\n\n')

            # Struct
            self.out.write(f'typedef struct {stripped_func_name}_data {{\n'\
                f'{"".join([f"{tab_char}{arg[1]} {arg[0]};{newline_char}" for arg in tmp_args.items()])}'\
            f'}} {stripped_func_name}_args_t; \n\n')


    def postamble(self):
        # Enum for call IDs
        self.out.write(f'typedef enum ANNOTATE_CALL_ID {{\n'\
            f'{f",{newline_char}".join([f"{tab_char}{self.my_enum_names[i]} = {self.call_id_offset + i}" for i in range(len(self.my_enum_names))])}\n'\
        f'}} annotate_call_id_t;\n\n')

        # Generic helper struct
        self.out.write('typedef struct annotation_data {\n'\
            '\tannotate_call_id_t type;\n'\
            '\tvoid* data;\n'\
        '} annotation_data_t;\n\n')


if __name__ == '__main__':
    cli_args = get_args()
    generator = HeaderGenerator(cli_args.specs, cli_args.header_destination)