/*
 * Mars Kolonisi Yaşam Destek Simülatörü
 * -------------------------------------
 * Karakter tabanlı (metin tabanlı) bir hayatta kalma simülasyonu.
 * Oyuncu, Mars'taki araştırma istasyonunda tek başına kalmış bir mühendisi
 * yönetir ve kurtarma ekibi gelene kadar 15 gün hayatta kalmaya çalışır.
 *
 * Derleme : gcc mars_kolonisi.c -o mars_kolonisi
 * Çalıştır: ./mars_kolonisi
 *
 * Not: Ekran çıktılarında Türkçe karakter kullanılmamıştır; böylece program
 * Windows komut satırı dahil her terminalde düzgün görüntülenir.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SON_GUN 15          /* Kurtarma ekibinin geleceği gün            */
#define MAKS_DEGER 100      /* Sağlık, enerji, oksijen, hasar üst sınırı  */
#define MIN_DEGER 0         /* Aynı değerlerin alt sınırı                 */

#define KESIF_ENERJI 15     /* G komutunun enerji maliyeti                */
#define KESIF_OKSIJEN 10    /* G komutunun oksijen maliyeti               */
#define URETIM_ENERJI 5     /* O komutunun enerji maliyeti                */
#define TAMIR_ENERJI 10     /* T komutunun enerji maliyeti                */
#define SIFRE_ENERJI 5      /* H komutunda her yanlış denemenin maliyeti  */
#define MAKS_DENEME 3       /* H komutundaki deneme hakkı                 */

int main(void)
{
    /* ---------------- Durum değişkenleri ---------------- */
    int saglik = 0, enerji = 0, oksijen = 0;
    int su = 0, yedekParca = 0, modulHasari = 0;
    int kilitMuhurlu = 0;           /* 0: açık, 1: mühürlü             */
    int gun = 1, tur = 0;           /* Gün 1'den, tur 0'dan başlar     */

    /* ------- Zorluk seviyesine göre değişen parametreler ------- */
    int suEsikYuksek = 0;           /* Durum iyiyken su bulma eşiği    */
    int suEsikDusuk = 0;            /* Durum kötüyken su bulma eşiği   */
    int parcaEsik = 0;              /* Yedek parça bulma eşiği (ek)    */
    int suBulmaMiktari = 0;         /* Bir keşifte bulunan su          */
    int yaralanmaHasari = 0;        /* Keşifte yaralanınca sağlık kaybı*/
    int firtinaTurSayisi = 0;       /* Kum fırtınası kaç tur sürer     */
    int firtinaHasari = 0;          /* Fırtınada tek olayın taban hasarı*/
    char zorlukAdi[10] = "";

    /* ------------------ Yardımcı değişkenler ------------------ */
    char secim, komut, tahmin, sifre;
    int c;                          /* Tampon temizliği için           */
    int zar, esik;
    int turGecti;                   /* Komut tur harcadı mı? (1/0)     */
    int oyunBitti = 0;              /* 0: devam, 1: kazandı, 2: kaybetti, 3: çıktı */
    int gecerli;
    int i;

    srand(time(NULL));

    printf("==================================================\n");
    printf("   MARS KOLONISI YASAM DESTEK SIMULATORU\n");
    printf("==================================================\n");
    printf("Istasyonda tek basinasin. Kurtarma ekibi %d gun sonra geliyor.\n", SON_GUN);
    printf("Hayatta kal!\n\n");

    /* ===== Zorluk seçimi: SWITCH-CASE + geçersizse tekrar sor (DO-WHILE) ===== */
    do {
        gecerli = 1;
        printf("Zorluk seviyesi secin (K: Kolay, N: Normal, Z: Zor): ");
        if (scanf(" %c", &secim) != 1) {
            return 0;               /* Girdi akışı kapandıysa çık      */
        }
        while ((c = getchar()) != '\n' && c != EOF) { }   /* Satırın kalanını at */

        switch (secim) {
            case 'k':
            case 'K':
                saglik = 100; enerji = 100; oksijen = 90;
                su = 20; yedekParca = 4; modulHasari = 10;
                suEsikYuksek = 60; suEsikDusuk = 35; parcaEsik = 25;
                suBulmaMiktari = 30; yaralanmaHasari = 8;
                firtinaTurSayisi = 3; firtinaHasari = 5;
                sprintf(zorlukAdi, "Kolay");
                break;
            case 'n':
            case 'N':
                saglik = 100; enerji = 90; oksijen = 80;
                su = 14; yedekParca = 3; modulHasari = 20;
                suEsikYuksek = 50; suEsikDusuk = 25; parcaEsik = 20;
                suBulmaMiktari = 24; yaralanmaHasari = 10;
                firtinaTurSayisi = 5; firtinaHasari = 8;
                sprintf(zorlukAdi, "Normal");
                break;
            case 'z':
            case 'Z':
                saglik = 90; enerji = 80; oksijen = 70;
                su = 8; yedekParca = 2; modulHasari = 30;
                suEsikYuksek = 40; suEsikDusuk = 15; parcaEsik = 15;
                suBulmaMiktari = 20; yaralanmaHasari = 15;
                firtinaTurSayisi = 7; firtinaHasari = 12;
                sprintf(zorlukAdi, "Zor");
                break;
            default:
                printf("[UYARI] Gecersiz zorluk seviyesi: '%c'. Lutfen K, N veya Z girin.\n", secim);
                gecerli = 0;
                break;
        }
    } while (gecerli == 0);

    printf("\nSecilen zorluk: %s\n", zorlukAdi);
    printf("Komutlar: G-Kesif  O-Oksijen uret  T-Tamir  D-Durum  U-Uyu\n");
    printf("          K-Kum firtinasi  H-Hava kilidi sifresi  Q-Cikis\n");

    /* ======================= ANA KOMUT DÖNGÜSÜ (DO-WHILE) ======================= */
    do {
        turGecti = 0;

        printf("\n--- Gun %d | Tur %d | Saglik %d | Enerji %d | Oksijen %d ---\n",
               gun, tur, saglik, enerji, oksijen);
        printf("Komut girin (G/O/T/D/U/K/H/Q): ");
        if (scanf(" %c", &komut) != 1) {
            komut = 'Q';            /* EOF gelirse çıkış say           */
        }
        while ((c = getchar()) != '\n' && c != EOF) { }

        switch (komut) {

            /* ------------------------- G: KEŞİF GÖREVİ ------------------------- */
            case 'g':
            case 'G':
                if (enerji < KESIF_ENERJI) {
                    printf("[IPTAL] Kesif icin en az %d enerji gerekir (mevcut: %d).\n",
                           KESIF_ENERJI, enerji);
                    break;
                }
                /* Su bulma eşiği, yola çıkmadan önceki duruma göre belirlenir */
                if (enerji > 40 && oksijen > 30) {
                    esik = suEsikYuksek;
                    printf("Durumun iyi, genis bir alani tarayabilirsin.\n");
                } else {
                    esik = suEsikDusuk;
                    printf("Durumun zayif, sadece yakin cevreyi tarayabilirsin.\n");
                }
                enerji -= KESIF_ENERJI;
                oksijen -= KESIF_OKSIJEN;

                zar = rand() % 100;
                if (zar < esik) {
                    su += suBulmaMiktari;
                    printf("Zar: %d / Su esigi: %d -> BASARILI (su bulundu)\n", zar, esik);
                    printf("Buz tabakasi buldun! +%d su (toplam: %d)\n", suBulmaMiktari, su);
                } else if (zar < esik + parcaEsik) {
                    yedekParca += 1;
                    printf("Zar: %d / Parca esigi: %d -> BASARILI (yedek parca bulundu)\n",
                           zar, esik + parcaEsik);
                    printf("Eski bir inis aracindan yedek parca sokuldu! +1 parca (toplam: %d)\n",
                           yedekParca);
                } else {
                    printf("Zar: %d / Esik: %d -> BASARISIZ\n", zar, esik + parcaEsik);
                    printf("Eli bos dondun.\n");
                }

                /* Yaralanma kontrolü: şans kötü YA DA modül çok hasarlı */
                zar = rand() % 100;
                if (zar < 10 || modulHasari > 70) {
                    saglik -= yaralanmaHasari;
                    printf("Yaralanma zari: %d / Esik: 10 (hasar: %d) -> YARALANDIN! -%d saglik\n",
                           zar, modulHasari, yaralanmaHasari);
                } else {
                    printf("Yaralanma zari: %d / Esik: 10 (hasar: %d) -> Saglam dondun.\n",
                           zar, modulHasari);
                }
                turGecti = 1;
                break;

            /* ------------------------- O: OKSİJEN ÜRET ------------------------- */
            case 'o':
            case 'O': {
                int miktar, kullanilan, artik, uretilen;

                if (enerji < URETIM_ENERJI) {
                    printf("[IPTAL] Oksijen uretimi icin en az %d enerji gerekir (mevcut: %d).\n",
                           URETIM_ENERJI, enerji);
                    break;
                }
                if (su < 2) {
                    printf("[IPTAL] Uretim icin en az 2 birim su gerekir (mevcut: %d).\n", su);
                    break;
                }
                if (oksijen >= MAKS_DEGER) {
                    printf("[IPTAL] Oksijen tanki zaten dolu (%d).\n", oksijen);
                    break;
                }

                /* Geçerli miktar girilene kadar tekrar sor */
                do {
                    printf("Kac birim su kullanilsin? (2 - %d): ", su);
                    if (scanf("%d", &miktar) != 1) {
                        miktar = -1;            /* Sayı olmayan giriş      */
                    }
                    while ((c = getchar()) != '\n' && c != EOF) { }

                    if (miktar > su) {
                        printf("[UYARI] Mevcut sudan (%d) fazla miktar girilemez.\n", su);
                    } else if (miktar < 2) {
                        printf("[UYARI] En az 2 birim su girilmelidir.\n");
                    }
                } while (miktar < 2 || miktar > su);

                /* Tek sayıda su girildiyse 1 birim artık olarak depoda kalır */
                artik = miktar % 2;
                kullanilan = miktar - artik;
                uretilen = (kullanilan / 2) * 3;

                if (artik == 1) {
                    printf("%d tek sayi: 1 birim su artik olarak depoda kaldi.\n", miktar);
                } else {
                    printf("%d cift sayi: suyun tamami kullanildi.\n", miktar);
                }

                su -= kullanilan;
                enerji -= URETIM_ENERJI;
                oksijen += uretilen;
                printf("%d su -> %d oksijen uretildi (2 su = 3 oksijen). Kalan su: %d\n",
                       kullanilan, uretilen, su);

                if (oksijen > MAKS_DEGER) {
                    printf("Tank kapasitesi %d. Fazla %d oksijen tahliye edildi.\n",
                           MAKS_DEGER, oksijen - MAKS_DEGER);
                }
                turGecti = 1;
                break;
            }

            /* ----------------------------- T: TAMİR ----------------------------- */
            case 't':
            case 'T': {
                int gerekenParca, onarim, harcanan;

                if (yedekParca == 0) {
                    printf("[IPTAL] Yedek parca yok, tamir yapilamaz.\n");
                    break;
                }
                if (enerji < TAMIR_ENERJI) {
                    printf("[IPTAL] Tamir icin en az %d enerji gerekir (mevcut: %d).\n",
                           TAMIR_ENERJI, enerji);
                    break;
                }
                if (modulHasari == 0) {
                    printf("[IPTAL] Modulde hasar yok, tamire gerek yok.\n");
                    break;
                }

                /* Hasar kademesi IF - ELSE IF - ELSE zinciriyle belirlenir */
                if (modulHasari <= 30) {
                    printf("Hasar seviyesi: HAFIF (%d)\n", modulHasari);
                    gerekenParca = 1;
                    onarim = 15;
                } else if (modulHasari <= 70) {
                    printf("Hasar seviyesi: ORTA (%d)\n", modulHasari);
                    gerekenParca = 2;
                    onarim = 30;
                } else {
                    printf("Hasar seviyesi: AGIR (%d)\n", modulHasari);
                    gerekenParca = 3;
                    onarim = 45;
                }

                /* Parça yetmiyorsa eldeki parçayla orantılı onarım yapılır */
                if (yedekParca >= gerekenParca) {
                    harcanan = gerekenParca;
                } else {
                    harcanan = yedekParca;
                    onarim = onarim * harcanan / gerekenParca;
                    printf("Yeterli parca yok (%d/%d). Kismi tamir yapiliyor.\n",
                           yedekParca, gerekenParca);
                }

                yedekParca -= harcanan;
                modulHasari -= onarim;
                enerji -= TAMIR_ENERJI;
                printf("%d parca harcandi, hasar %d azaldi. Kalan parca: %d\n",
                       harcanan, onarim, yedekParca);
                turGecti = 1;
                break;
            }

            /* -------------------------- D: DURUM RAPORU -------------------------- */
            case 'd':
            case 'D':
                printf("\n=========== DURUM RAPORU ===========\n");
                printf("Zorluk       : %s\n", zorlukAdi);
                printf("Gun / Tur    : %d / %d\n", gun, tur);
                printf("Saglik       : %d\n", saglik);
                printf("Enerji       : %d\n", enerji);
                printf("Oksijen      : %d\n", oksijen);
                printf("Su           : %d birim\n", su);
                printf("Yedek parca  : %d\n", yedekParca);
                printf("Modul hasari : %d\n", modulHasari);
                printf("Hava kilidi  : %s\n", kilitMuhurlu == 1 ? "MUHURLU" : "ACIK");
                printf("------------------------------------\n");

                /* Her kritik değer için ayrı uyarı */
                if (oksijen < 25) {
                    printf("[KRITIK] Oksijen seviyesi cok dusuk!\n");
                }
                if (saglik < 30) {
                    printf("[KRITIK] Saglik durumu kritik!\n");
                }
                if (modulHasari > 70) {
                    printf("[KRITIK] Modul agir hasarli!\n");
                }
                if (enerji < 20) {
                    printf("[KRITIK] Enerji tukenmek uzere!\n");
                }

                /* Tüm değerler güvenliyse tek bir && koşulu */
                if (oksijen >= 25 && saglik >= 30 && modulHasari <= 70 && enerji >= 20) {
                    printf(">>> TUM SISTEMLER NORMAL <<<\n");
                }
                printf("====================================\n");
                /* D komutu tur harcamaz */
                break;

            /* --------------------------- U: UYU / DİNLEN --------------------------- */
            case 'u':
            case 'U': {
                int enerjiArtisi = 20 + (MAKS_DEGER - enerji) / 4;   /* Yorgunken daha çok dinlenir */
                int saglikArtisi = 10 + saglik / 10;

                enerji += enerjiArtisi;
                oksijen -= 4;                       /* Uyurken de nefes alınır */
                printf("Dinlendin. Enerji +%d. Uyurken 4 oksijen tuketildi.\n", enerjiArtisi);

                /* İç içe IF: oksijen azsa sağlık artmaz, azalır */
                if (oksijen < 20) {
                    saglik -= 5;
                    printf("Oksijen az (%d) oldugu icin iyi uyuyamadin: -5 saglik.\n", oksijen);
                } else {
                    if (saglik < MAKS_DEGER) {
                        saglik += saglikArtisi;
                        printf("Saglik +%d.\n", saglikArtisi);
                    } else {
                        printf("Sagligin zaten tam.\n");
                    }
                }
                turGecti = 1;
                break;
            }

            /* ------------------- K: KUM FIRTINASI (FOR DÖNGÜSÜ) ------------------- */
            case 'k':
            case 'K': {
                int hasarliTur = 0, toplamSaglikKaybi = 0, toplamOksijenKaybi = 0;
                int carpan, kayip, korunmaEsigi;

                printf("\n*** KUM FIRTINASI BASLADI! (%d tur surecek) ***\n", firtinaTurSayisi);

                for (i = 1; i <= firtinaTurSayisi; i++) {
                    /* Çift numaralı turlarda rüzgar şiddetlenir: hasar 2 katı */
                    if (i % 2 == 0) {
                        carpan = 2;
                    } else {
                        carpan = 1;
                    }
                    kayip = firtinaHasari * carpan;

                    /* Korunma şansı oyuncunun durumuna bağlıdır */
                    if (modulHasari < 50 && saglik > 50) {
                        korunmaEsigi = 40;
                    } else {
                        korunmaEsigi = 20;
                    }

                    zar = rand() % 100;
                    printf("Tur %d%s | Zar: %d / Korunma esigi: %d -> ",
                           i, carpan == 2 ? " (RUZGAR SIDDETLENDI x2)" : "", zar, korunmaEsigi);

                    if (zar < korunmaEsigi) {
                        printf("KORUNDUN, zarar yok.\n");
                    } else if (zar < korunmaEsigi + 20) {
                        saglik -= kayip;
                        toplamSaglikKaybi += kayip;
                        hasarliTur++;
                        printf("Ucan kayalar carpti: -%d saglik.\n", kayip);
                    } else if (zar < korunmaEsigi + 45) {
                        if (kilitMuhurlu == 1) {
                            printf("Oksijen sizintisi engellendi (hava kilidi muhurlu).\n");
                            continue;       /* Sızıntı olayı bu tur atlanır */
                        }
                        oksijen -= kayip;
                        toplamOksijenKaybi += kayip;
                        hasarliTur++;
                        printf("Hava kilidinden oksijen sizdi: -%d oksijen.\n", kayip);
                    } else {
                        modulHasari += kayip;
                        hasarliTur++;
                        printf("Modul hasar aldi: +%d hasar.\n", kayip);
                    }

                    /* Değerleri sınırla ve hayati değerleri kontrol et */
                    if (saglik < MIN_DEGER) {
                        saglik = MIN_DEGER;
                    }
                    if (oksijen < MIN_DEGER) {
                        oksijen = MIN_DEGER;
                    }
                    if (modulHasari > MAKS_DEGER) {
                        modulHasari = MAKS_DEGER;
                    }
                    if (saglik == 0 || oksijen == 0) {
                        printf("!!! Hayati degerler sifira dustu, firtina sonlandi. !!!\n");
                        break;      /* Döngüyü hemen bitir */
                    }
                }

                printf("--- Firtina Ozeti ---\n");
                printf("Hasar alinan tur      : %d\n", hasarliTur);
                printf("Toplam saglik kaybi   : %d\n", toplamSaglikKaybi);
                printf("Toplam oksijen kaybi  : %d\n", toplamOksijenKaybi);
                turGecti = 1;
                break;
            }

            /* --------------- H: HAVA KİLİDİ ŞİFRESİ (DO-WHILE DÖNGÜSÜ) --------------- */
            case 'h':
            case 'H': {
                int deneme = 0;

                if (kilitMuhurlu == 1) {
                    printf("[IPTAL] Hava kilidi zaten muhurlu.\n");
                    break;
                }
                if (enerji < SIFRE_ENERJI) {
                    printf("[IPTAL] Sifre denemesi icin en az %d enerji gerekir (mevcut: %d).\n",
                           SIFRE_ENERJI, enerji);
                    break;
                }

                sifre = 'A' + (gun % 26);   /* Şifre güne göre değişir */
                printf("IPUCU: Sifre, alfabede A harfinden bugunun gun sayisi (%d) kadar ilerideki harftir.\n",
                       gun);

                do {
                    printf("Deneme %d/%d - Harf girin (A-Z): ", deneme + 1, MAKS_DENEME);
                    if (scanf(" %c", &tahmin) != 1) {
                        tahmin = '?';
                    }
                    while ((c = getchar()) != '\n' && c != EOF) { }

                    /* Küçük harfi büyük harfe çevir (aritmetik) */
                    if (tahmin >= 'a' && tahmin <= 'z') {
                        tahmin = tahmin - 'a' + 'A';
                    }
                    if (tahmin < 'A' || tahmin > 'Z') {
                        printf("[UYARI] Sadece A-Z arasi bir harf girin. Deneme hakki dusmedi.\n");
                        continue;   /* Koşula gider, deneme sayılmaz */
                    }

                    deneme++;
                    if (tahmin != sifre) {
                        enerji -= SIFRE_ENERJI;
                        if (tahmin < sifre) {
                            printf("Yanlis! Alfabede ILERI git. (-%d enerji)\n", SIFRE_ENERJI);
                        } else {
                            printf("Yanlis! Alfabede GERI git. (-%d enerji)\n", SIFRE_ENERJI);
                        }
                    }
                } while (tahmin != sifre && deneme < MAKS_DENEME);

                if (tahmin == sifre) {
                    kilitMuhurlu = 1;
                    printf("Dogru sifre: '%c'. Hava kilidi MUHURLENDI!\n", sifre);
                } else {
                    modulHasari += 20;
                    printf("%d deneme de basarisiz (sifre '%c' idi). Kilit zorlandi: modul hasari +20.\n",
                           MAKS_DENEME, sifre);
                }
                turGecti = 1;
                break;
            }

            /* ------------------------------ Q: ÇIKIŞ ------------------------------ */
            case 'q':
            case 'Q':
                printf("Simulasyondan cikiliyor...\n");
                oyunBitti = 3;
                break;

            /* --------------------------- TANIMSIZ KOMUT --------------------------- */
            default:
                printf("[UYARI] Tanimsiz komut: '%c'. Tur harcanmadi.\n", komut);
                break;
        }

        /* ============== HER KOMUTTAN SONRA SINIRLANDIRMA (IF) ============== */
        if (saglik < MIN_DEGER)       saglik = MIN_DEGER;
        if (saglik > MAKS_DEGER)      saglik = MAKS_DEGER;
        if (enerji < MIN_DEGER)       enerji = MIN_DEGER;
        if (enerji > MAKS_DEGER)      enerji = MAKS_DEGER;
        if (oksijen < MIN_DEGER)      oksijen = MIN_DEGER;
        if (oksijen > MAKS_DEGER)     oksijen = MAKS_DEGER;
        if (modulHasari < MIN_DEGER)  modulHasari = MIN_DEGER;
        if (modulHasari > MAKS_DEGER) modulHasari = MAKS_DEGER;

        /* ======================== ZAMAN AKIŞI ======================== */
        if (turGecti == 1) {
            tur++;

            /* Her 3 turda bir yeni gün başlar */
            if (tur % 3 == 0) {
                gun++;
                if (gun > SON_GUN) {
                    oyunBitti = 1;  /* 15. gün tamamlandı */
                } else {
                    printf("\n>>> GUN %d BASLADI. Gece boyunca 8 oksijen tuketildi.\n", gun);
                    oksijen -= 8;
                    if (modulHasari > 70) {
                        saglik -= 5;
                        printf(">>> Modul agir hasarli (%d): gece soguktu, -5 saglik.\n",
                               modulHasari);
                    }
                    if (oksijen < MIN_DEGER) oksijen = MIN_DEGER;
                    if (saglik < MIN_DEGER)  saglik = MIN_DEGER;
                }
            }

            /* ======================= OYUN SONU KONTROLÜ ======================= */
            if (saglik <= 0 || oksijen <= 0) {
                oyunBitti = 2;
            }
        }

    } while (oyunBitti == 0);

    /* ============================ SONUÇ RAPORU ============================ */
    {
        int hayattaKalinanGun = gun - 1;    /* Tamamlanan gün sayısı */
        int puan = (hayattaKalinanGun * 100) + saglik + oksijen - modulHasari;

        printf("\n==================================================\n");
        if (oyunBitti == 1) {
            printf("  KURTARMA EKIBI GELDI! %d GUN HAYATTA KALDIN!\n", SON_GUN);
        } else if (oyunBitti == 2) {
            if (saglik <= 0) {
                printf("  OYUN BITTI: Saglik tukendi.\n");
            } else {
                printf("  OYUN BITTI: Oksijen tukendi.\n");
            }
        } else {
            printf("  SIMULASYON YARIDA BIRAKILDI.\n");
        }
        printf("==================================================\n");
        printf("Zorluk               : %s\n", zorlukAdi);
        printf("Hayatta kalinan gun  : %d\n", hayattaKalinanGun);
        printf("Toplam tur           : %d\n", tur);
        printf("Saglik               : %d\n", saglik);
        printf("Enerji               : %d\n", enerji);
        printf("Oksijen              : %d\n", oksijen);
        printf("Su                   : %d\n", su);
        printf("Yedek parca          : %d\n", yedekParca);
        printf("Modul hasari         : %d\n", modulHasari);
        printf("Hava kilidi          : %s\n", kilitMuhurlu == 1 ? "MUHURLU" : "ACIK");
        printf("--------------------------------------------------\n");
        printf("PUAN = (%d x 100) + %d + %d - %d = %d\n",
               hayattaKalinanGun, saglik, oksijen, modulHasari, puan);
        printf("==================================================\n");
    }

    return 0;
}
