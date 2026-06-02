# Hotel Management Console App

Ung dung quan ly khach san chay tren console, duoc viet bang ngon ngu C. Project phu hop cho bai tap/bao cao mon Ky thuat lap trinh C, tap trung vao thao tac file, mang struct, menu console va xu ly nghiep vu co ban cua khach san.

## Chuc nang

### Quan ly phong
- Xem danh sach phong
- Them phong moi
- Xoa phong khi phong khong co khach dang o
- Tu dong dong bo trang thai phong voi booking dang check-in

### Quan ly khach hang
- Xem danh sach khach hang
- Them khach hang moi
- Quan ly hang thanh vien: Bronze, Silver, Gold
- Xoa khach hang co kiem tra lien ket booking/hoa don

### Check-in / Check-out
- Tao booking check-in
- Tu dong lay ngay check-in hien tai
- Nhap ngay check-out du kien
- Huy booking dang o
- Check-out va tinh tien phong, giam gia, dich vu
- Hien bang xac nhan check-in/check-out tren console

### Dich vu
- Goi dich vu cho booking dang o
- Cong don so luong neu goi trung dich vu
- Xoa dich vu da goi nham truoc khi check-out
- Danh muc dich vu doc tu `data/services.txt`

### Hoa don
- Xem hoa don theo booking
- In hoa don ra file `.txt`
- Luu lich su doanh thu vao `data/bills.txt`
- Hoa don xuat ra thu muc `output/`

## Cau truc thu muc

```text
.
|-- main.c
|-- Makefile
|-- README.md
|-- include/
|   |-- bill.h
|   |-- booking.h
|   |-- customer.h
|   |-- menu.h
|   |-- room.h
|   |-- service.h
|   |-- types.h
|   `-- utils.h
|-- src/
|   |-- bill.c
|   |-- booking.c
|   |-- customer.c
|   |-- menu.c
|   |-- room.c
|   |-- service.c
|   `-- utils.c
|-- data/
|   |-- bills.txt
|   |-- bookings.txt
|   |-- customers.txt
|   |-- rooms.txt
|   |-- services.txt
|   `-- used_services.txt
`-- output/
    `-- HDxxx.txt
```

## Yeu cau

- GCC hoac MinGW GCC
- Make hoac `mingw32-make`
- Windows PowerShell, CMD, hoac terminal tuong duong

## Build

### Cach 1: dung Makefile

Tren Windows voi MinGW:

```powershell
mingw32-make
```

Neu moi truong cua ban dung lenh `make`:

```powershell
make
```

### Cach 2: bien dich truc tiep bang GCC

```powershell
gcc -Wall -Wextra -std=c11 -Iinclude main.c src\room.c src\customer.c src\booking.c src\service.c src\bill.c src\menu.c src\utils.c -o hotel.exe
```

## Chay chuong trinh

```powershell
.\hotel.exe
```

Hoac chay qua Makefile:

```powershell
mingw32-make run
```

## Cac lenh huu ich

Kiem tra cu phap:

```powershell
mingw32-make syntax
```

Xoa file build tam va file `.exe`:

```powershell
mingw32-make clean
```

Build lai tu dau:

```powershell
mingw32-make rebuild
```

## Du lieu

Du lieu duoc luu bang file text/CSV trong thu muc `data/`.

```text
rooms.txt          Danh sach phong
customers.txt      Danh sach khach hang
bookings.txt       Danh sach booking
services.txt       Danh muc dich vu
used_services.txt  Dich vu da goi theo booking
bills.txt          Lich su hoa don/doanh thu
```

Vi du `data/rooms.txt`:

```text
P101,Single,300000,0
P102,Single,350000,0
P103,Single,400000,1
```

Trang thai phong:

```text
0 = Trong
1 = Dang thue
```

Trang thai booking:

```text
1 = Dang o
2 = Da tra
3 = Da huy
```

## Luu y khi dua len GitHub

Nen commit:

- Ma nguon trong `src/`
- Header trong `include/`
- `main.c`
- `Makefile`
- `README.md`
- Du lieu mau trong `data/` neu can demo

Khong nen commit:

- File object `*.o`
- File build `hotel.exe`
- File hoa don sinh ra trong `output/*.txt` neu khong muon luu ket qua demo

Neu chua co `.gitignore`, co the tao voi noi dung:

```gitignore
*.o
hotel.exe
output/*.txt
```

## Tac gia

Project quan ly khach san bang C cho muc tieu hoc tap va thuc hanh lap trinh console.
