# Mars Kolonisi Yaşam Destek Simülatörü

C dili ile geliştirilmiş, karakter tabanlı (metin tabanlı) bir hayatta kalma simülasyonu. Oyuncu, Mars'taki küçük bir araştırma istasyonunda tek başına kalmış bir mühendisi yönetir ve kurtarma ekibi gelene kadar **15 gün** hayatta kalmaya çalışır.

Bu proje, **Algoritma ve Programlama** dersi ödevi kapsamında hazırlanmıştır. Program yalnızca C dilinin temel yapılarını kullanır: aritmetik ve mantıksal operatörler, `if / else if / else` zincirleri, `switch-case` (fall-through ile), `for` ve `do-while` döngüleri, `break` ve `continue`.

## İçindekiler

- [Özellikler](#özellikler)
- [Kurulum ve Çalıştırma](#kurulum-ve-çalıştırma)
- [Komutlar](#komutlar)
- [Zorluk Seviyeleri](#zorluk-seviyeleri)
- [Oyun Kuralları](#oyun-kuralları)
- [Kullanılan C Yapıları](#kullanılan-c-yapıları)
- [Örnek Çıktı](#örnek-çıktı)
- [Proje Yapısı](#proje-yapısı)

## Özellikler

- Tek karakterli, büyük/küçük harf duyarsız komut sistemi
- Üç zorluk seviyesi (Kolay, Normal, Zor); seviye, başlangıç değerlerini ve olay eşiklerini değiştirir
- Sağlık, enerji, oksijen, su, yedek parça, modül hasarı, hava kilidi, gün ve tur takibi
- `srand(time(NULL))` ve `rand() % 100` ile rastgele olaylar; her zar ve eşik değeri ekrana yazdırılır
- `for` döngüsüyle kum fırtınası simülasyonu (`break` ve `continue` kullanımıyla)
- `do-while` döngüsüyle sınırlı denemeli hava kilidi şifresi
- Oyun nasıl biterse bitsin sonuç raporu ve puan hesabı

## Kurulum ve Çalıştırma

Herhangi bir C derleyicisi (GCC, Clang, MinGW) yeterlidir. Harici kütüphane gerekmez.

```bash
git clone https://github.com/MavlyudaAlmazova/mars-kolonisi-simulatoru.git
cd mars-kolonisi-simulatoru
gcc mars_kolonisi.c -o mars_kolonisi
./mars_kolonisi
```

Windows (MinGW) için:

```bash
gcc mars_kolonisi.c -o mars_kolonisi.exe
mars_kolonisi.exe
```

## Komutlar

| Komut | İşlev | Tur harcar mı? |
|:-----:|-------|:--------------:|
| `G` | Keşif görevi: enerji 15, oksijen 10 azalır. Su bulma, yedek parça bulma veya eli boş dönme sonuçlarından biri gerçekleşir. Yaralanma ihtimali vardır. | Evet |
| `O` | Oksijen üretimi: her 2 birim su, 3 birim oksijene dönüşür. Tek sayıda su girilirse 1 birim artık olarak depoda kalır. | Evet |
| `T` | Tamir: yedek parça harcanarak modül hasarı azaltılır. Harcanan parça ve onarım miktarı hasar kademesine göre değişir. | Evet |
| `D` | Durum raporu: tüm değerler ve kritik uyarılar gösterilir. | Hayır |
| `U` | Uyu / dinlen: enerji ve sağlık artar, oksijen tüketimi devam eder. | Evet |
| `K` | Kum fırtınası: zorluğa göre 3, 5 veya 7 tur süren fırtına simüle edilir. | Evet |
| `H` | Hava kilidi şifresi: 3 deneme hakkıyla kilit mühürlenmeye çalışılır. | Evet |
| `Q` | Simülasyondan çıkış ve sonuç raporu. | — |

Tanımsız bir komut girildiğinde program uyarı verir ve tur harcanmaz. Enerjisi yetersiz olan bir komut iptal edilir ve yine tur harcanmaz.

## Zorluk Seviyeleri

| Parametre | Kolay (K) | Normal (N) | Zor (Z) |
|-----------|:---------:|:----------:|:-------:|
| Başlangıç sağlığı | 100 | 100 | 90 |
| Başlangıç enerjisi | 100 | 90 | 80 |
| Başlangıç oksijeni | 90 | 80 | 70 |
| Başlangıç suyu | 20 | 14 | 8 |
| Yedek parça | 4 | 3 | 2 |
| Modül hasarı | 10 | 20 | 30 |
| Su bulma eşiği (iyi / kötü durum) | 60 / 35 | 50 / 25 | 40 / 15 |
| Yedek parça eşiği (ek) | 25 | 20 | 15 |
| Bulunan su miktarı | 30 | 24 | 20 |
| Yaralanma hasarı | 8 | 10 | 15 |
| Kum fırtınası süresi | 3 tur | 5 tur | 7 tur |
| Fırtına taban hasarı | 5 | 8 | 12 |

Geçersiz bir zorluk seviyesi girildiğinde seçim tekrar istenir.

## Oyun Kuralları

**Zaman akışı:** Tur harcayan her komut tur sayacını 1 artırır. Her 3 turda bir (`tur % 3 == 0`) yeni gün başlar. Gün başında oksijen 8 azalır; modül hasarı 70'in üzerindeyse ayrıca 5 sağlık kaybedilir.

**Sınırlandırma:** Sağlık, enerji, oksijen ve modül hasarı her komuttan sonra `if` yapılarıyla 0–100 aralığına çekilir.

**Oyun sonu:**

- Sağlık veya oksijen 0'a düşerse oyun kaybedilir.
- 15. gün tamamlanırsa kurtarma ekibi gelir ve oyun kazanılır.
- `Q` komutuyla oyundan çıkılabilir.

Her durumda hayatta kalınan gün, toplam tur, son durum değerleri ve puan içeren bir sonuç raporu yazdırılır.

**Puan formülü:**

```
Puan = (hayattaKalinanGun × 100) + saglik + oksijen − modulHasari
```

Gün katsayısının 100 seçilmesinin nedeni, hayatta kalınan her günün kalan sağlık ve oksijenden (en fazla 100'er puan) daha değerli olmasıdır. Modül hasarı ise istasyonun bırakıldığı durumu yansıttığı için puandan düşülür.

## Kullanılan C Yapıları

| Yapı | Kullanıldığı yer |
|------|------------------|
| `switch-case` + fall-through | Zorluk seçimi ve komut sistemi (`case 'g': case 'G':`) |
| `default` | Geçersiz zorluk ve tanımsız komut uyarıları |
| `if / else if / else` | Keşif sonuçları, tamir kademeleri, fırtına olayları, oyun sonu |
| İç içe `if` | Dinlenmede oksijen kontrolü |
| `&&`, `\|\|` | Su bulma eşiği, yaralanma koşulu, "TÜM SİSTEMLER NORMAL" kontrolü, şifre döngüsü koşulu |
| `%` (mod) | Tek/çift su kontrolü, çift numaralı fırtına turları, gün geçişi, şifre üretimi |
| `for` + `break` + `continue` | Kum fırtınası |
| `do-while` | Ana komut döngüsü, zorluk seçimi, su miktarı girişi, hava kilidi şifresi |

## Örnek Çıktı

```
--- Gun 1 | Tur 0 | Saglik 100 | Enerji 90 | Oksijen 80 ---
Komut girin (G/O/T/D/U/K/H/Q): g
Durumun iyi, genis bir alani tarayabilirsin.
Zar: 60 / Parca esigi: 70 -> BASARILI (yedek parca bulundu)
Eski bir inis aracindan yedek parca sokuldu! +1 parca (toplam: 4)
Yaralanma zari: 70 / Esik: 10 (hasar: 20) -> Saglam dondun.

--- Gun 1 | Tur 1 | Saglik 100 | Enerji 75 | Oksijen 70 ---
Komut girin (G/O/T/D/U/K/H/Q): o
Kac birim su kullanilsin? (2 - 14): 3
3 tek sayi: 1 birim su artik olarak depoda kaldi.
2 su -> 3 oksijen uretildi (2 su = 3 oksijen). Kalan su: 12
```

## Proje Yapısı

```
.
├── mars_kolonisi.c   # Programın tüm kaynak kodu
├── rapor.pdf         # Kod mantığını açıklayan proje raporu
└── README.md         # Bu dosya
```

## Lisans

Bu proje eğitim amaçlı hazırlanmıştır.
