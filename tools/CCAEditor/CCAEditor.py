import os
import argparse

def display_content(content):
    print("Offset     | Hexadecimal Representation | Printable Text")
    print("----------------------------------------------------------")

    for i in range(0, len(content), 16):
        line = content[i:i+16]
        hex_line = ' '.join(format(b, '02X') for b in line)
        printable_text = ''.join(chr(byte) if 32 <= byte <= 126 else '.' for byte in line)
        print(f"{i:010X} | {hex_line.ljust(48)} | {printable_text}")

def display_strings(content):
    print("Strings in the CCA Archive:")
    print("--------------------------")
    strings = content.split(b'\x00')
    printed = False
    for string in strings:
        try:
            decoded_string = string.decode('utf-8')
            printable_string = ''.join(c for c in decoded_string if ' ' <= c <= '~')
            if printable_string:
                if printed:
                    print()
                print(printable_string)
                printed = True
        except UnicodeDecodeError:
            pass

def clear_console():
    input("Press Enter to clear the Console")
    print("\033[2J\033[H")

def save_changes(filename, content):
    with open(filename, 'wb') as file:
        file.write(content)
    print("Changes saved successfully.")

def search_and_replace(content, old_text, new_text):
    # Convert old and new text to bytes
    old_bytes = old_text.encode('utf-8')
    new_bytes = new_text.encode('utf-8')

    # Check if old text exists in content
    if old_bytes not in content:
        print("Error: The text to be replaced does not exist in the CCA Archive.")
        return content

    # Perform search and replace on binary data
    modified_content = content.replace(old_bytes, new_bytes)
    
    return modified_content

def count_occurrences(content, word):
    occurrences = []
    offset = 0
    while True:
        offset = content.find(word.encode('utf-8'), offset)
        if offset == -1:
            break
        occurrences.append(offset)
        offset += len(word)
    return occurrences

def main():
    parser = argparse.ArgumentParser(description='Process CCA archive.')
    print("Usage: CCAEditor.py enter then the Archive name.\nOr CCAEditor.py example.CCA to directly pass the archive to the Editor.\n")
    parser.add_argument('filename', nargs='?', help='CCA archive name')
    args = parser.parse_args()

    if args.filename:
        filename = args.filename
    else:
        filename = input("Enter CCA Archive name: ")

    if not filename.endswith('.CCA'):
        filename += '.CCA'

    if not os.path.isfile(filename):
        print(f"Error: Archive '{filename}' not found.")
        return

    with open(filename, 'rb') as file:
        content = file.read()

    print("CCAEditor ver. 1.2beta")
    print("-------------------------")
    print(f"Editing Archive: {filename}")
    print(f"Archive size: {len(content)} bytes")

    while True:
        print("\nEnter 'hexedit' to display/edit the CCA Archive.")
        print("Enter 'replace' to replace printable text.")
        print("Enter 'count' to count occurrences of a word.")
        print("Enter 'strings' to display strings in the CCA Archive.")
        print("Enter 'save' to save changes and exit.")
        print("Enter 'help' to display help how to modify CCA Archive.")
        print("Enter 'clear' to clear the Console output")
        print("Enter 'exit' to exit without saving.")
        
        choice = input("Your choice: ").lower()

        if choice == 'help':
            print("\nHow to modify CCA Archive:")
            print("- Make sure to always make a backup of the Archive you want to modify!")
            print("- If you replace values, don't make the new value longer than the old one.")
            print("- Make sure you do not enlarge the filesize of the CCA Archive.")
            break
        elif choice == 'hexedit':
            display_content(content)
            try:
                offset_input = input("Enter offset to modify (in hexadecimal), or press Enter to continue viewing: ")
                if offset_input.strip() == '':
                    continue
                offset = int(offset_input, 16)
                if 0 <= offset < len(content):
                    new_value = int(input("Enter new hexadecimal value: "), 16)
                    # Since content is bytes, convert to bytearray to edit
                    content_array = bytearray(content)
                    content_array[offset] = new_value
                    content = bytes(content_array)
                    print("Offset updated successfully.")
                else:
                    print("Error: Offset out of bounds.")
            except ValueError:
                print("Error: Invalid input.")
        elif choice == 'replace':
            old_text = input("Enter the text to search for: ")
            new_text = input("Enter the replacement text: ")
            if len(new_text) > len(old_text):
                print("Error: Replacement text cannot be longer than the original text.")
            else:
                content = search_and_replace(content, old_text, new_text)
                print("Text replaced successfully.")
        elif choice == 'count':
            word = input("Enter the word to count: ")
            occurrences = count_occurrences(content, word)
            print(f"'{word}' is found {len(occurrences)} times in the archive.")
            clear_console()
            if occurrences:
                print("Offsets:")
                for i, offset in enumerate(occurrences):
                    print(f"{i+1}. 0x{offset:08X}")
        elif choice == 'strings':
            display_strings(content)
        elif choice == 'clear':
            clear_console()
        elif choice == 'save':
            save_changes(filename, content)
            break
        elif choice == 'exit':
            print("Exiting without saving.")
            clear_console()
            break
        else:
            print("Error: Invalid choice.")

if __name__ == "__main__":
    main()
