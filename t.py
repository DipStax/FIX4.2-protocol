import os

TARGET_EXTENSIONS = {'.cpp', '.hpp', '.inl', '.cs'}
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
    comment_lines = 0
    in_block_comment = False

    try:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as file:
            for line in file:
                total_lines += 1
                stripped = line.strip()

                if stripped == '':
                    empty_lines += 1
                    continue

                # Handle block comments
                if in_block_comment:
                    comment_lines += 1
                    if "*/" in stripped:
                        in_block_comment = False
                    continue

                # Start of block comment
                if "/*" in stripped:
                    comment_lines += 1
                    if "*/" not in stripped:
                        in_block_comment = True
                    continue

                # Single-line comments (C++/C#/CMake)
                if stripped.startswith("//") or stripped.startswith("#"):
                    comment_lines += 1
                    continue

    except Exception as e:
        print(f"Error reading file {filepath}: {e}")

    return total_lines, empty_lines, comment_lines

def count_files_and_lines(start_path='.'):
    file_count = 0
    total_lines = 0
    total_empty_lines = 0
    total_comment_lines = 0

    for root, dirs, files in os.walk(start_path):
        dirs[:] = [d for d in dirs if d not in EXCLUDED_DIRS]

        for file in files:
            if is_target_file(file):
                filepath = os.path.join(root, file)
                file_count += 1
                lines, empty, comments = analyze_file(filepath)
                total_lines += lines
                total_empty_lines += empty
                total_comment_lines += comments

    return file_count, total_lines, total_empty_lines, total_comment_lines

if __name__ == '__main__':
    path_to_scan = '.'
    count, lines, empty_lines, comment_lines = count_files_and_lines(path_to_scan)
    print(f"Files counted: {count}")
    print(f"Total lines: {lines}")
    print(f"Empty lines: {empty_lines}")
    print(f"Comment lines: {comment_lines}")
    print(f"Written lines: {lines - empty_lines - comment_lines}")
