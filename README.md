# Hotel Management Console App (Simplified)

Ung dung quan ly khach san viet bang C, da rut gon cho muc tieu hoc phan Ky thuat lap trinh C.

## Chuc nang chinh

1. Quan ly phong
- Xem danh sach phong
- Them phong
- Xoa phong

2. Quan ly khach hang
- Xem danh sach khach hang
- Them khach hang (co hang thanh vien)
- Xoa khach hang (co rang buoc lien ket booking/hoa don)

3. Check-in / Check-out
- Tao check-in (tu dong lay ngay check-in hien tai)
- Check-out va tinh tien phong + giam gia + dich vu
- Huy booking dang o

4. Dich vu
- Goi dich vu cho booking dang o
- Danh muc dich vu doc dong tu `data/services.txt`

5. Hoa don
- Xem hoa don theo booking
- In hoa don `.txt` cho booking da check-out
- Luu doanh thu vao `data/bills.txt`

## Cau truc thu muc

```text
include/      Header files
src/          Source files
data/         Du lieu CSV
output/       Hoa don xuat ra (.txt)
main.c        Diem vao chuong trinh
Makefile      Build script
```

## Build

### Cach 1: dung make

```powershell
make
```

### Cach 2: dung gcc truc tiep

```powershell
gcc -Wall -Wextra -std=c11 -Iinclude main.c src\room.c src\customer.c src\booking.c src\service.c src\bill.c src\menu.c src\utils.c -o hotel.exe
```

## Run

```powershell
.\hotel.exe
```

## Luu y

- Du lieu mau nam trong thu muc `data/`.
- Khong can commit `hotel.exe` (da bo qua trong `.gitignore`).
- Hoa don xuat ra `output/*.txt` co the tao lai tu chuong trinh nen cung da duoc bo qua.
