#include <stdio.h>

// Membuat tipe data struct untuk menyimpan koordinat titik
struct Point {
  double x;
  double y;
};

// Membuat tipe data struct untuk menyimpan poligon segi banyak
struct Polygon {
  struct Point *vertices; // array dinamis untuk menyimpan koordinat sudut poligon
  int num_vertices; // jumlah sudut pada poligon
};

// Fungsi untuk menentukan apakah titik p berada di dalam poligon poly
int isInsidePolygon(struct Point p, struct Polygon poly) {
  int i, j, c = 0;
  for (i = 0, j = poly.num_vertices - 1; i < poly.num_vertices; j = i++) {
    double xi = poly.vertices[i].x, yi = poly.vertices[i].y;
    double xj = poly.vertices[j].x, yj = poly.vertices[j].y;

    // Periksa apakah titik p ada di antara garis vertikal yang melewati xi dan xj
    if (((yi > p.y) != (yj > p.y)) && (p.x < (xj - xi) * (p.y - yi) / (yj - yi) + xi)) {
      c = !c;
    }
  }
  return c;
}
/*
int main() {
    // Inisialisasi poligon dengan menggunakan array dinamis untuk koordinat titik
    struct Polygon poly;
    poly.num_vertices = 6;
    poly.vertices = malloc(poly.num_vertices * sizeof(struct Point));
    poly.vertices[0].x = 0; poly.vertices[0].y = 0;
    poly.vertices[1].x = 4; poly.vertices[1].y = 0;
    poly.vertices[2].x = 4; poly.vertices[2].y = 4;
    poly.vertices[3].x = 2; poly.vertices[3].y = 6;
    poly.vertices[4].x = 0; poly.vertices[4].y = 4;
    poly.vertices[5].x = 3; poly.vertices[5].y = 5;

    // Inisialisasi titik yang akan diuji
    struct Point p;
    p.x = 3; p.y = 4;

    // Panggil fungsi isInsidePolygon untuk menentukan apakah titik p berada di dalam poligon
    if (isInsidePolygon(p, poly)) {
      printf("Titik (%g, %g) berada di dalam poligon\n", p.x, p.y);
    } else {
      printf("Titik (%g, %g) berada di luar poligon\n", p.x, p.y);
    }

    // Bebaskan memory yang digunakan untuk menyimpan koordinat titik pada poligon
    free(poly.vertices);

    return 0;
}
*/