# ls(1) – Phiên bản đơn giản hóa bằng C

Bài tập giữa kỳ môn **Lập trình hệ thống UNIX**: cài đặt lại lệnh `ls(1)` của UNIX từ đầu bằng ngôn ngữ C, dựa trên manual page `ls(1)` của NetBSD 10.1 (27/10/2024) được cung cấp trong đề bài.

Qua bài tập, em thực hành các thao tác với hệ thống file UNIX (`opendir`/`readdir`, `stat`/`lstat`, `readlink`), xử lý tham số dòng lệnh, sắp xếp, định dạng đầu ra và xử lý lỗi.

## Thông tin

| | |
|---|---|
| **Sinh viên** | Ngô Phạm Viết Long |
| **MSSV** | 24IT150 |
| **Lớp / Khoa** | 24JIT / Khoa Khoa học máy tính |
| **Giảng viên** | TS. Nguyễn Nhật Ân |
| **Thời gian** | Tháng 10 năm 2026 |
| **Repository** | https://github.com/LongNPV-IT/NgoPhamVietLong_24IT150_midterm |

## Mục lục

1. [Môi trường](#1-môi-trường)
2. [Biên dịch và chạy](#2-biên-dịch-và-chạy)
3. [Cấu trúc dự án](#3-cấu-trúc-dự-án)
4. [Tính năng đã cài đặt](#4-tính-năng-đã-cài-đặt)
5. [Minh họa kết quả](#5-minh-họa-kết-quả)
6. [Kiểm thử](#6-kiểm-thử)
7. [Hạn chế](#7-hạn-chế)
8. [Kết luận](#8-kết-luận)
9. [Tài liệu tham khảo](#9-tài-liệu-tham-khảo)

## 1. Môi trường

- **Hệ điều hành** phát triển và kiểm thử: NetBSD 10.1 (máy ảo)
- **Trình biên dịch:** `cc` (GCC/Clang), chuẩn C99, cờ `-Wall -Wextra`
- **Công cụ:** `make`, `git`

<img width="863" height="377" alt="01-environment" src="https://github.com/user-attachments/assets/127eb162-6df8-4854-ab4a-4d165debaf3b" />

## 2. Biên dịch và chạy

```sh
git clone https://github.com/LongNPV-IT/NgoPhamVietLong_24IT150_midterm.git
cd NgoPhamVietLong_24IT150_midterm
make            # biên dịch, tạo file thực thi ./ls
./ls -l         # chạy thử
make clean      # xóa file thực thi
```

> **Lưu ý:** dùng `./ls` để chạy chương trình của project, tránh nhầm với lệnh `ls` của hệ thống.

Cú pháp: `./ls [option ...] [file ...]`. Các option cần được truyền **riêng từng cái** (ví dụ `./ls -l -a`); chưa hỗ trợ gộp như `-la`.

<img width="824" height="143" alt="02-build" src="https://github.com/user-attachments/assets/725b3e34-8b2a-4835-bdce-5631e6c04224" />

## 3. Cấu trúc dự án

```text
.
├── README.md
├── Makefile
├── .gitignore
├── test_full.sh          # script kiểm thử
├── include/
│   ├── options.h
│   ├── directory.h
│   ├── fileinfo.h
│   └── sorting.h
└── src/
    ├── main.c
    ├── options.c
    ├── directory.c
    ├── fileinfo.c
    └── sorting.c
```

| Module | Chức năng |
|---|---|
| `main.c` | Điểm vào chương trình: khởi tạo `Options`, phân tích tham số, phân loại operand (file/directory), in tiêu đề thư mục khi có nhiều operand, trả về exit status. |
| `options.c` / `options.h` | Định nghĩa cấu trúc `Options` và hàm `parse_options`. Xử lý các option ghi đè nhau (`-l`/`-n`, `-c`/`-u`, `-q`/`-w`, `-R`/`-d`, `-h`/`-k`). |
| `directory.c` / `directory.h` | Mở và đọc thư mục, lọc file ẩn theo `-a`/`-A`, thu thập entry, sắp xếp, in kết quả; hỗ trợ đệ quy `-R`. |
| `fileinfo.c` / `fileinfo.h` | Lấy thông tin file bằng `stat`/`lstat`, định dạng chuỗi quyền (`rwx`, `s`/`S`, `t`/`T`), owner/group, kích thước (`-h`, `-k`), thời gian, symbolic link, ký hiệu `-F`, in long format. |
| `sorting.c` / `sorting.h` | Sắp xếp theo tên (mặc định), kích thước (`-S`), thời gian (`-t`), đảo ngược (`-r`), không sắp xếp (`-f`). |

## 4. Tính năng đã cài đặt

Hỗ trợ đầy đủ các option trong manual được cung cấp:

| Option | Chức năng |
|:---:|---|
| `-A` | Liệt kê mọi entry trừ `.` và `..` |
| `-a` | Bao gồm các entry bắt đầu bằng dấu chấm |
| `-c` | Dùng thời gian thay đổi trạng thái (ctime) để sắp xếp (`-t`) hoặc in (`-l`) |
| `-d` | Liệt kê thư mục như file thường, không đi vào bên trong; không đi theo symbolic link ở operand |
| `-F` | Thêm ký hiệu sau tên: `/` thư mục, `*` thực thi, `@` symlink, `\|` FIFO |
| `-f` | Không sắp xếp |
| `-h` | Hiển thị kích thước dạng dễ đọc (K, M, G, …) |
| `-i` | In số inode |
| `-k` | Hiển thị kích thước theo kilobyte |
| `-l` | Định dạng danh sách dài |
| `-n` | Như `-l` nhưng hiển thị UID/GID dạng số |
| `-q` | In ký tự không in được trong tên file thành `?` |
| `-R` | Liệt kê đệ quy các thư mục con |
| `-r` | Đảo ngược thứ tự sắp xếp |
| `-S` | Sắp xếp theo kích thước, file lớn nhất trước |
| `-s` | Hiển thị số block của mỗi file (có dòng `total` khi xuất ra terminal) |
| `-t` | Sắp xếp theo thời gian sửa đổi, mới nhất trước |
| `-u` | Dùng thời gian truy cập (atime) |
| `-w` | In thô ký tự không in được |

### Quy tắc ghi đè giữa các option

Theo manual, với các cặp sau, option xuất hiện **sau cùng** được áp dụng:

| Cặp option | Ý nghĩa |
|---|---|
| `-l` / `-n` | Hiển thị tên hay UID/GID dạng số |
| `-c` / `-u` | Loại thời gian được dùng |
| `-q` / `-w` | Cách in ký tự không in được |
| `-R` / `-d` | Đệ quy hay coi thư mục như file |
| `-h` / `-k` | Đơn vị hiển thị kích thước |

### Xử lý lỗi và exit status

Khi operand không tồn tại, không mở được thư mục hoặc option không hợp lệ, chương trình in thông báo lỗi và trả về exit status `1`; chạy thành công trả về `0`.

## 5. Minh họa kết quả

### 5.1. Liệt kê cơ bản

`./ls` liệt kê thư mục hiện tại:

<img width="821" height="337" alt="03-basic-listing" src="https://github.com/user-attachments/assets/37b0efbf-3cd4-4bfe-9169-5dba0bd68a6f" />

`./ls -a` / `./ls -A` hiển thị file ẩn:

<img width="245" height="275" alt="04-hidden-files" src="https://github.com/user-attachments/assets/6aef2471-9290-4707-8e6b-40a24efda8c9" />

### 5.2. Định dạng danh sách dài

`./ls -l` hiển thị quyền, số link, owner, group, kích thước, thời gian sửa đổi và tên file:

<img width="485" height="215" alt="05-long-format" src="https://github.com/user-attachments/assets/67fa8275-893f-4f1a-8966-50c83a1d6d74" />

Kích thước dễ đọc và số block (`-h`, `-k`, `-s`, `-i`):

<img width="502" height="423" alt="06-size-and-blocks" src="https://github.com/user-attachments/assets/5eb7b166-a0fd-4cf1-9105-6ca4c5da06d6" />

`-F` thêm ký hiệu loại file:

<img width="167" height="119" alt="07-classify" src="https://github.com/user-attachments/assets/d135d31a-63f2-44ee-b904-9ed71ebde266" />

### 5.3. Sắp xếp

Các kiểu sắp xếp `-S`, `-t`, `-r`, `-f`:

<img width="494" height="412" alt="08-sorting" src="https://github.com/user-attachments/assets/5cbd4de0-acb8-40c2-8bbf-38aa39b7c129" />

Chọn loại thời gian với `-u` và `-c`:

<img width="491" height="215" alt="09-time-options" src="https://github.com/user-attachments/assets/598bc72c-d971-47ba-a51d-76f4dd242593" />

### 5.4. Thư mục, đệ quy và symbolic link

`-R` liệt kê đệ quy, `-d` chỉ hiển thị chính thư mục:

<img width="210" height="357" alt="10-recursive-and-d" src="https://github.com/user-attachments/assets/5d98a07a-4227-43ad-ba15-72d82cb35d55" />

Symbolic link: không dùng `-d` thì đi vào thư mục đích; dùng `-d` thì hiển thị chính symlink:

<img width="577" height="175" alt="11-symlink" src="https://github.com/user-attachments/assets/fbe5356b-6adc-42b6-bb59-abf92c8e94b4" />

### 5.5. Xử lý lỗi

Operand không tồn tại cho thông báo lỗi và exit status `1`:

<img width="317" height="135" alt="12-error-exit-status" src="https://github.com/user-attachments/assets/54401118-9c41-4221-b728-2dd8636cfd51" />

## 6. Kiểm thử

`test_full.sh` là bộ kiểm thử hồi quy. Script tự build nếu chưa có `./ls`, tạo dữ liệu kiểm thử tạm (tự dọn khi kết thúc), đếm số test Passed/Failed và trả về exit status khác `0` nếu có test thất bại.

```sh
chmod +x test_full.sh
./test_full.sh > test_results.txt 2>&1
cat test_results.txt
```

Các nhóm được kiểm tra: từng option riêng lẻ, tổ hợp option, quy tắc ghi đè, symbolic link, nhiều operand, option đứng sau operand, xử lý lỗi và exit status, chuyển hướng output, Setuid/Setgid, FIFO, và so sánh tham khảo với `/bin/ls` (không so sánh từng byte).

<img width="706" height="526" alt="13-test-results" src="https://github.com/user-attachments/assets/090d4678-c41e-4da9-85e5-b1beedc22890" />

## 7. Hạn chế

- Chỉ cài đặt các option trong phạm vi manual được cung cấp; không có màu sắc hay option mở rộng của GNU `ls`.
- Option phải được truyền riêng từng cái (`./ls -l -a`); chưa hỗ trợ gộp như `-la`.
- Sau `--`, operand bắt đầu bằng dấu `-` chưa được xử lý như tên file.
- Ký hiệu whiteout (`%`) và socket (`=`) của `-F` chưa được cài đặt/kiểm thử đầy đủ.
- Chưa kiểm thử mọi loại filesystem đặc biệt của NetBSD.
- Project được kiểm thử trên NetBSD 10.1; trên hệ khác (ví dụ Linux với `-std=c99` nghiêm ngặt) có thể cần thêm cờ biên dịch.
- Output không đảm bảo giống từng byte với `ls` của hệ thống.

## 8. Kết luận

Chương trình đã cài đặt các option theo manual `ls(1)` được yêu cầu, tổ chức thành nhiều module `.c`/`.h` rõ ràng, có Makefile, script kiểm thử và được lưu trữ trên GitHub. Qua bài tập, em hiểu rõ hơn về các system call thao tác file, cách tổ chức chương trình C nhiều file và cách kiểm thử một công cụ dòng lệnh.

### Quản lý mã nguồn

File `.gitignore` loại trừ file thực thi `ls`, file object `.o`, core dump và file tạm `*.tmp`.

<img width="959" height="563" alt="14-github-repo" src="https://github.com/user-attachments/assets/8f4ba520-adb1-4cc9-8206-8b22b91469d6" />

<img width="335" height="60" alt="15-commit-history" src="https://github.com/user-attachments/assets/fa3034a1-4932-411c-88dd-c5e4b6aca47a" />

## 9. Tài liệu tham khảo

- Manual page `ls(1)`, NetBSD 10.1 (27/10/2024), được cung cấp trong đề bài.
- Tài liệu môn học Lập trình hệ thống UNIX.
- Manual các hàm: `stat(2)`, `opendir(3)`, `readdir(3)`, `readlink(2)`, `strftime(3)`.
