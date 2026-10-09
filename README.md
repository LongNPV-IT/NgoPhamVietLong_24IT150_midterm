# Lệnh UNIX `ls` - Phiên Bản Đơn Giản Hóa

Lập trình lệnh `ls(1)` của UNIX từ đầu như một bài tập giữa kỳ môn Lập trình hệ thống UNIX. Dự án được xây dựng dựa trên manual `ls(1)` của NetBSD được cung cấp trong bài tập.

Project tập trung vào các thao tác hệ thống file UNIX, xử lý command-line arguments, thông tin file và directory, symbolic link, permission, sorting và các khái niệm lập trình ở mức hệ thống.

## Thông Tin Tác Giả

- **Sinh viên:** Ngô Phạm Viết Long
- **MSSV:** 24IT150
- **Môn học:** Lập trình hệ thống UNIX
- **GitHub:** [LongNPV-IT/NgoPhamVietLong_24IT150_midterm](https://github.com/LongNPV-IT/NgoPhamVietLong_24IT150_midterm)

## Yêu Cầu Hệ Thống

- **Hệ điều hành:** NetBSD hoặc hệ UNIX-like tương thích
- **Compiler:** `cc`, GCC hoặc Clang hỗ trợ C99
- **C Standard:** C99
- **Build tool:** `make`
- **Git:** Dùng để quản lý source code và làm việc với repository

Project được phát triển và kiểm thử chính trên NetBSD 10.1.

## Cài Đặt và Sử Dụng

Clone repository và biên dịch:

```sh
git clone https://github.com/LongNPV-IT/NgoPhamVietLong_24IT150_midterm.git
cd NgoPhamVietLong_24IT150_midterm
make
```

Chạy chương trình:

```sh
./ls
./ls -l
./ls -a
./ls -F
./ls -R
```

Xóa binary sau khi build:

```sh
make clean
```

> Dùng `./ls` để chạy chương trình của project, tránh nhầm với lệnh `ls` của hệ thống.

## Cấu Trúc Dự Án

```text
.
├── README.md
├── Makefile
├── .gitignore
├── include/
│   ├── directory.h
│   ├── fileinfo.h
│   ├── options.h
│   └── sorting.h
└── src/
    ├── main.c
    ├── options.c
    ├── directory.c
    ├── fileinfo.c
    └── sorting.c
```

## Mô Tả Các Module

### `main.c`

Điểm bắt đầu của chương trình. Khởi tạo cấu hình, phân tích options và operands, điều phối xử lý file/directory và xử lý exit status.

### `options.c`

Phân tích command-line options và lưu trạng thái các option trong cấu trúc `Options`.

### `directory.c`

Mở và đọc directory, lọc entries, lấy thông tin file, sắp xếp và in nội dung; hỗ trợ recursive listing với `-R`.

### `fileinfo.c`

Xử lý thông tin filesystem như file type, permission, owner, group, file size, inode, link count, timestamps, symbolic link và file type suffix.

### `sorting.c`

Sắp xếp directory entries theo tên, kích thước hoặc thời gian; hỗ trợ đảo ngược thứ tự.

## Các Tính Năng Đã Cài Đặt

Bản cài đặt hỗ trợ các tùy chọn trong phạm vi manual `ls(1)` được cung cấp:

| Option | Chức năng                                                |
| ------ | -------------------------------------------------------- |
| `-A`   | Liệt kê mọi entry trừ `.` và `..`                        |
| `-a`   | Bao gồm các entry bắt đầu bằng `.`                       |
| `-c`   | Sử dụng change time (`ctime`)                            |
| `-d`   | Hiển thị directory như một file thay vì liệt kê nội dung |
| `-F`   | Thêm ký hiệu phân biệt loại file                         |
| `-f`   | Không sắp xếp kết quả                                    |
| `-h`   | Hiển thị kích thước theo dạng dễ đọc                     |
| `-i`   | Hiển thị số inode                                        |
| `-k`   | Hiển thị kích thước theo kilobyte                        |
| `-l`   | Hiển thị thông tin ở long format                         |
| `-n`   | Như `-l` nhưng hiển thị UID/GID dạng số                  |
| `-q`   | Thay ký tự không in được bằng `?`                        |
| `-R`   | Liệt kê directory theo kiểu đệ quy                       |
| `-r`   | Đảo ngược thứ tự sắp xếp                                 |
| `-S`   | Sắp xếp theo kích thước                                  |
| `-s`   | Hiển thị số filesystem blocks                            |
| `-t`   | Sắp xếp theo thời gian                                   |
| `-u`   | Sử dụng access time (`atime`)                            |
| `-w`   | Hiển thị tên file ở dạng raw                             |

### Ký Hiệu Loại File

Option `-F` thêm ký hiệu sau tên file theo loại file:

| Ký hiệu | Loại file       |
| ------- | --------------- |
| `/`     | Directory       |
| `*`     | Executable file |
| `@`     | Symbolic link   |
| `\|`    | FIFO            |

Ví dụ:

```sh
./ls -F
```

## Build

Makefile sử dụng `cc` với các tùy chọn:

```make
CC = cc
CFLAGS = -Wall -Wextra -std=c99
CPPFLAGS = -Iinclude
```

Biên dịch và xóa binary:

```sh
make
make clean
```

Binary sau khi build là `./ls`.

## Cách Sử Dụng

### Cú Pháp Cơ Bản

```sh
./ls
./ls src
./ls include
./ls Makefile
./ls README.md
./ls Makefile README.md
./ls src include
```

Có thể truyền nhiều file và directory làm operands:

```sh
./ls Makefile src
./ls Makefile src include README.md
```

Khi operand là directory, chương trình liệt kê nội dung của directory.

### Ví Dụ Theo Tùy Chọn

Long format:

```sh
./ls -l
```

Long format hiển thị permission, link count, owner, group, file size, timestamp và file name.

Hiển thị file ẩn, inode hoặc kích thước:

```sh
./ls -a
./ls -A
./ls -i
./ls -h
./ls -l -h
./ls -k
./ls -l -k
```

Hiển thị directory như file, liệt kê đệ quy hoặc thay đổi thứ tự:

```sh
./ls -d src
./ls -R
./ls -R src
./ls -S
./ls -S -r
./ls -t
./ls -t -r
./ls -f
./ls -r
```

Sử dụng thời gian truy cập hoặc change time:

```sh
./ls -l -u
./ls -l -c
```

Hiển thị filesystem blocks:

```sh
./ls -s
./ls -s > output.txt
```

Khi output trực tiếp ra terminal, `-s` hiển thị thêm dòng `total`. Khi redirect output, dòng `total` không được in vào file.

### Symbolic Link, Permission Đặc Biệt và FIFO

Tạo symbolic link và xem ký hiệu loại file:

```sh
ln -s README.md readme-link
ln -s include include-link
./ls -F
```

Khi chạy `./ls readme-link`, chương trình hiển thị symbolic link. Khi chạy `./ls include-link`, chương trình xử lý target directory của symbolic link. Dùng `-d` để hiển thị chính symbolic link:

```sh
./ls -d include-link
```

Chương trình xử lý các permission bit đặc biệt như Setuid và Setgid:

```sh
chmod 4755 special-test
./ls -l special-test

chmod 2755 special-test
./ls -l special-test
```

Tạo và kiểm tra FIFO:

```sh
mkfifo test-fifo
./ls -l test-fifo
./ls -F test-fifo
```

### Options Sau Operand và Option Separator

Project hỗ trợ options xuất hiện sau operand:

```sh
./ls include-link -F
./ls -d include-link -F
```

`--` đánh dấu kết thúc phần options; các argument phía sau được xử lý như operands:

```sh
./ls -- src
```

### Thứ Tự Ưu Tiên Của Options

Với một số option có hành vi ghi đè lẫn nhau, option xuất hiện sau cùng được áp dụng:

| Options           | Hành vi                                                     |
| ----------------- | ----------------------------------------------------------- |
| `-l -n` / `-n -l` | Option sau cùng quyết định hiển thị tên hay UID/GID dạng số |
| `-c -u` / `-u -c` | Option thời gian xuất hiện sau cùng được áp dụng            |
| `-q -w` / `-w -q` | Option xuất hiện sau cùng được áp dụng                      |
| `-R -d` / `-d -R` | Option xuất hiện sau cùng được áp dụng                      |

## Chi Tiết Cài Đặt

### Đọc Directory

Quy trình xử lý directory gồm mở directory, đọc từng entry, kiểm tra file ẩn, lấy thông tin filesystem, lưu và sắp xếp entries, sau đó hiển thị kết quả.

### Lấy Thông Tin File

Chương trình lấy thông tin file type, permission, link count, UID, GID, file size, modification time, change time, access time, inode và filesystem blocks. `lstat()` được sử dụng khi cần thông tin của chính symbolic link thay vì target.

### Sắp Xếp và Định Dạng

Các chế độ sắp xếp được hỗ trợ gồm theo tên, kích thước (`-S`), thời gian (`-t`), đảo ngược (`-r`) và không sắp xếp (`-f`). Permission được chuyển từ mode bits sang dạng như `-rwxr-xr-x`; các bit Setuid, Setgid và Sticky được thể hiện bằng `s`, `St` hoặc `T` tùy trường hợp. Với `-h`, kích thước được chuyển sang dạng dễ đọc, ví dụ `30568 bytes` có thể hiển thị thành `29.9K`.

## Xử Lý Lỗi

Chương trình xử lý một số lỗi phổ biến như file hoặc directory không tồn tại, không thể mở directory, option không hợp lệ và không thể lấy filesystem information.

Ví dụ khi operand không tồn tại:

```sh
./ls not-exist
echo $?
```

Tài liệu dự án ghi nhận exit status là `1` trong trường hợp lỗi và `0` khi chạy thành công.

## Kiểm Thử

Project có script `test_full.sh` để chạy bộ regression test trên NetBSD hoặc môi trường UNIX tương thích. Script tự build chương trình bằng `make` nếu chưa có executable `./ls`, tạo fixtures tạm để kiểm thử và tự dọn chúng khi kết thúc.

Chạy script từ thư mục gốc của project:

```sh
cd /home/npvlong/ls-midterm
chmod +x test_full.sh
./test_full.sh
```

Để lưu cả output chuẩn và lỗi vào file rồi xem kết quả:

```sh
./test_full.sh > test_results.txt 2>&1
cat test_results.txt
```

Script in số test Passed/Failed/Total và trả về exit status khác `0` nếu có test thất bại. So sánh output với `/bin/ls` chỉ mang tính tham khảo, không phải so sánh byte-for-byte.

Các nhóm chức năng được kiểm tra gồm:

- Basic listing và các options `-a`, `-A`, `-d`, `-F`, `-i`, `-l`, `-n`, `-h`, `-k`, `-s`, `-f`, `-r`, `-S`, `-t`, `-c`, `-u`, `-q`, `-w`, `-R`
- Symbolic links và multiple operands
- Options sau operands và `--`
- Error handling, exit status và output redirection
- Setuid, Setgid và FIFO
- Các tổ hợp options riêng lẻ, precedence, fixture/edge cases và so sánh tham khảo với `/bin/ls`

Build cuối cùng được kiểm tra bằng cách chạy `make clean`, `make` và các lệnh `./ls`, `./ls -l`, `./ls -F`, `./ls -R`; tài liệu ghi nhận không có compiler warning hoặc error.

## Hạn Chế

Đây là phiên bản simplified `ls(1)` phục vụ mục đích học tập và phạm vi bài Midterm, không nhằm thay thế hoàn toàn lệnh `ls` của hệ điều hành.

- Chỉ triển khai tập options trong phạm vi manual được cung cấp.
- Không đảm bảo tương thích hoàn toàn với mọi hành vi của `ls` phiên bản hệ thống.
- Chưa kiểm thử đầy đủ tất cả filesystem đặc biệt của NetBSD.
- Whiteout (`%`) chưa được kiểm thử đầy đủ; socket (`=`) chưa được kiểm thử riêng trong bộ test hiện tại.
- Không triển khai các chức năng ngoài phạm vi manual như màu sắc output hoặc options mở rộng riêng của GNU `ls`.
- Đây không phải implementation production-level; một số cú pháp command-line nâng cao có thể khác với `ls` hệ thống.

## Git và GitHub

Binary `ls`, object files `.o` và các file test tạm thời như symbolic link, FIFO, test output không thuộc source chính thức của project và không được commit.

```sh
git status
git add Makefile README.md .gitignore src include
git commit -m "Complete ls midterm project"
git push
```

## Giấy Phép

Tài liệu nguồn xác định đây là project giáo dục cho môn Lập trình hệ thống

## Tài Liệu Tham Khảo

- Manual `ls(1)` của NetBSD được cung cấp trong bài tập
- Tài liệu môn học Lập trình hệ thống UNIX
- Tài liệu về UNIX filesystem và system programming
- Tài liệu C Standard Library và POSIX/UNIX system interfaces

Cập nhật lần cuối: Tháng 10 năm 2026.
