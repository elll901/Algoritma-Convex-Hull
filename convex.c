#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "insidePoligon2.c"
#define EARTH_RADIUS 6371
#define PI 3.141592653

typedef struct
{
    char markas[50];
    double lat;
    double lon;
} Point;

typedef struct
{
  struct Point *vertices; // array dinamis untuk menyimpan koordinat sudut poligon
  int num_vertices; // jumlah sudut pada poligon
} Polygon;


// use cross product to get orientation (clockwise (area -) or counterclockwise (area +))
double cross_product(Point a, Point b, Point c)
{
    double out;
    // vector ab x ac
    // (ab x, ab y) x (ac x, ac y) = (ab x * ac y) - (ab y * ac x)
    if (((b.lon - a.lon) * (c.lat - b.lat)) - ((c.lon - b.lon) * (b.lat - a.lat)) > 0) {
        out = -1;
    }
    else {
        Point temp = a;
        a = b;
        b = temp;
        double p12, p13, p23;
        p12 = sqrt(pow((a.lat - b.lat), 2) + pow((a.lon - b.lon), 2));
        p13 = sqrt(pow((a.lat - c.lat), 2) + pow((a.lon - c.lon), 2));
        p23 = sqrt(pow((b.lat - c.lat), 2) + pow((b.lon - c.lon), 2));
        out = acos((pow(p12, 2) + pow(p13, 2) - pow(p23, 2)) / (2 * p12 * p13));
    }
    return out;
}

// haversine formula to convert latitude and longitude distance into km
double haversine(Point a, Point b)
{
    // convert latitude and longitude to radian
    // lat / 180 * PI
    double lat_a_rad = a.lat / 180 * PI;
    double lat_b_rad = b.lat / 180 * PI;
    double lon_a_rad = a.lon / 180 * PI;
    double lon_b_rad = b.lon / 180 * PI;

    // distance between 2 lat and 2 lon
    double dist_lat = lat_b_rad - lat_a_rad;
    double dist_lon = lon_b_rad - lon_a_rad;

    // calculate haversine of half the chord length
    double r = pow(sin(dist_lat / 2), 2) + (pow(sin(dist_lon / 2), 2) * cos(lat_a_rad) * cos(lat_b_rad));

    // calculate great circle distance
    double c = 2 * atan2(sqrt(r), sqrt(1 - r));
    double dist = EARTH_RADIUS * c;
    return dist;
}

int convex_hull(Point total[100], int num, int idx_markas[20])
{
    // find most bottom point (axis y : latitude)
    int idx_bottom = 0;
    for (int i = 0; i < num; i++)
    {
        if (total[i].lat < total[idx_bottom].lat)
        {
            idx_bottom = i;
        }
    }
    int idx_right = 0;
    for (int i = 0; i < num; i++)
    {
        if (total[i].lon > total[idx_bottom].lon)
        {
            idx_right = i;
        }
    }
    int idx_prev = idx_right; // right karena bakal nyari next nya ke kiri (clockwise)
    int idx_curr = idx_bottom;
    double len_segment = 0;
    int j = 0;

    printf("\nPerbatasan Efektif Negara Api:\n");
    do
    {
        printf("%s -> ", total[idx_curr].markas);

        idx_markas[j] = idx_curr; /// jadi idx current
        double terkiri = 0;
        double kiri = 0;
        int next = 0;
        /// kemungkinan dibawah ini jg dihapus
        /// idx_curr = (idx_prev + 1) % num; // modulo is for making sure idx not exceed num
        for (int i = 0; i < num; i++)
        {
            // calculate haversine distance between prev and curr point
            double haversine_dist_i = haversine(total[idx_curr], total[i]);

            // if haversine distance is greater than 2500 km, skip this point
            if (haversine_dist_i > 2500)
            {
                continue;
            }

            kiri = cross_product(total[idx_prev], total[idx_curr], total[i]);

            if (kiri > terkiri)
            {
                next = i;
                terkiri = kiri;
            }
        }
        idx_prev = idx_curr;
        idx_curr = next;

        ////idx_prev = idx_curr;        /// kemungkinan dihapus
        j += 1;
    } while (idx_curr != idx_bottom);
    printf("%s\n", total[idx_curr].markas);
    return j;
}


int main()
{

    // input filename
    char filename[30];
    printf("Masukkan file markas: ");
    scanf("%s", filename);

    // initialize num and array of Point
    int num = 0;
    Point total[20];

    // get the array of points
    FILE* F = fopen(filename, "r");
    char buffer[100];
    fgets(buffer, 100, F);
    buffer[strlen(buffer) - 1] = '\0';
    char* token;
    while (fgets(buffer, 100, F) != NULL)
    {
        token = strtok(buffer, ",");
        strcpy(total[num].markas, token);
        token = strtok(NULL, ",");
        total[num].lat = atof(token);
        token = strtok(NULL, ",");
        total[num].lon = atof(token);
        num += 1;
    }
    fclose(F);

    // convex hull
    int idx_markas[20];
    int j = convex_hull(total, num, idx_markas);
    idx_markas[j] = idx_markas[0];
    double len_segment = 0;
    for (int i = 0; i < j; i++)
    {
        len_segment += haversine(total[idx_markas[i]], total[idx_markas[i + 1]]);
    }
    printf("\nPanjang Perbatasan Efektif Negara Api: %f km\n", len_segment);

    // Masukin data ke Polygon
    struct Polygon poly;
    poly.num_vertices = j;
    poly.vertices = malloc(poly.num_vertices*sizeof(Point));
    for(int i = 0;i<j;i++){
        poly.vertices[i].x = total[idx_markas[i]].lon;
        poly.vertices[i].y = total[idx_markas[i]].lat;
    }
    
    // Data Udah Masuk Saatnya Cek
    int found, count = 1;
    for (int i = 0; i < 20; i++) {
        found = 0;
        for (int k = 0; k < j; k++) {
            if (i == idx_markas[k]) {
                found = 1;
                break;
            }
        }
        if (!found) {
            struct Point p;
            p.x = total[i].lon; p.y = total[i].lat;
            if (!isInsidePolygon(p,poly)) {
                printf("%d. %s\n", count, total[i].markas);
                count++;
            }
        }
    }

    // write file
    FILE* F2 = fopen("segment.csv", "a");
    fprintf(F2, "Bujur,Lintang\n");
    for (int i = 0; i < j + 1; i++)
    {
        fprintf(F2, "%f,%f\n", total[idx_markas[i]].lat, total[idx_markas[i]].lon);
    }
}