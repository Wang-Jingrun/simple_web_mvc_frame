import os


def count_lines(directory, exclude_folder):
    total_lines = 0

    for root, dirs, files in os.walk(directory):
        # 排除指定文件夹
        if exclude_folder in dirs:
            dirs.remove(exclude_folder)

        for file in files:
            if file.endswith('.cpp') or file.endswith('.h') or file.endswith('.inl') or file.endswith('.hpp'):
                file_path = os.path.join(root, file)
                with open(file_path, 'r', encoding='utf-8') as f:
                    lines = f.readlines()
                    total_lines += len(lines)

    return total_lines


# 示例用法
directory = './'  # 替换为实际的目录路径
exclude_folder = 'cmake-build-debug-cygwin-build-debug'  # 替换为需要排除的文件夹名称

line_count = count_lines(directory, exclude_folder)
print("Total lines:", line_count)
