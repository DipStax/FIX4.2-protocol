import os

TARGET_EXTENSIONS = {'.cpp', '.hpp', '.inl'}
TARGET_FILENAMES = {'CMakeLists.txt'}
EXCLUDED_DIRS = {'build'}

def is_target_file(filename):
    return (
        os.path.splitext(filename)[1] in TARGET_EXTENSIONS or 
        filename in TARGET_FILENAMES
    )

def analyze_file(filepath):
    total_lines = 0
    empty_lines = 0
    try:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as file:
            for line in file:
                total_lines += 1
                if line.strip() == '':
                    empty_lines += 1
    except Exception as e:
        print(f"Error reading file {filepath}: {e}")
    return total_lines, empty_lines

def count_files_and_lines(start_path='.'):
    file_count = 0
    total_lines = 0
    total_empty_lines = 0

    for root, dirs, files in os.walk(start_path):
        dirs[:] = [d for d in dirs if d not in EXCLUDED_DIRS]

        for file in files:
            if is_target_file(file):
                filepath = os.path.join(root, file)
                file_count += 1
                lines, empty = analyze_file(filepath)
                total_lines += lines
                total_empty_lines += empty

    return file_count, total_lines, total_empty_lines

if __name__ == '__main__':
    path_to_scan = '.'
    count, lines, empty_lines = count_files_and_lines(path_to_scan)
    print(f"Files counted: {count}")
    print(f"Total lines: {lines}")
    print(f"Empty lines: {empty_lines}")
