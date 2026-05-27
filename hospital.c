#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Patient {
    int id;
    char name[100];
    int age;
    char disease[100];
};

void addPatient();
void viewPatients();
void searchPatient();
void updatePatient();
void deletePatient();


int readLine(char *buf, int size) {
    if (!fgets(buf, size, stdin)) return 0;
    buf[strcspn(buf, "\r\n")] = '\0';
    return 1;
}

/* Read an integer from a line */
int readInt(int *out) {
    char buf[32];
    if (!readLine(buf, sizeof(buf))) return 0;
    return sscanf(buf, "%d", out) == 1;
}

int main() {
    int choice;

    while (1) {
        printf("\n====== HOSPITAL MANAGEMENT SYSTEM ======\n");
        printf("1. Add Patient\n");
        printf("2. View Patients\n");
        printf("3. Search Patient\n");
        printf("4. Update Patient\n");
        printf("5. Delete Patient\n");
        printf("6. Exit\n");
        printf("\nEnter Choice: ");

        if (!readInt(&choice)) {
            printf("Invalid input!\n");
            continue;
        }

        switch (choice) {
            case 1: addPatient();    break;
            case 2: viewPatients();  break;
            case 3: searchPatient(); break;
            case 4: updatePatient(); break;
            case 5: deletePatient(); break;
            case 6: printf("Goodbye!\n"); exit(0);
            default: printf("Invalid Choice! Enter 1-6.\n");
        }
    }
}

/* ==================== ADD PATIENT ==================== */
void addPatient() {
    FILE *fp;
    struct Patient p, existing;

    printf("\nEnter Patient ID: ");
    if (!readInt(&p.id)) { printf("Invalid ID!\n"); return; }

    /* Check for duplicate ID */
    fp = fopen("patients.txt", "r");
    if (fp != NULL) {
        while (fscanf(fp, "%d|%99[^|]|%d|%99[^\n]",
               &existing.id, existing.name,
               &existing.age, existing.disease) == 4) {
            if (existing.id == p.id) {
                printf("Error: Patient ID %d already exists!\n", p.id);
                fclose(fp);
                return;
            }
        }
        fclose(fp);
    }

    printf("Enter Full Name  : ");
    if (!readLine(p.name, sizeof(p.name)) || strlen(p.name) == 0) {
        printf("Invalid name!\n"); return;
    }

    printf("Enter Age        : ");
    if (!readInt(&p.age)) { printf("Invalid age!\n"); return; }
    if (p.age <= 0 || p.age > 150) { printf("Error: Age must be 1-150!\n"); return; }

    printf("Enter Disease    : ");
    if (!readLine(p.disease, sizeof(p.disease)) || strlen(p.disease) == 0) {
        printf("Invalid disease!\n"); return;
    }

    fp = fopen("patients.txt", "a");
    if (fp == NULL) { printf("Error: Cannot open file!\n"); return; }

    fprintf(fp, "%d|%s|%d|%s\n", p.id, p.name, p.age, p.disease);
    fclose(fp);
    printf("\nPatient Added Successfully!\n");
}

/* ==================== VIEW PATIENTS ==================== */
void viewPatients() {
    FILE *fp;
    struct Patient p;
    int count = 0;

    fp = fopen("patients.txt", "r");
    if (fp == NULL) { printf("\nNo records found!\n"); return; }

    printf("\n%-6s %-25s %-6s %s\n", "ID", "Name", "Age", "Disease");
    printf("------------------------------------------------------------\n");

    while (fscanf(fp, "%d|%99[^|]|%d|%99[^\n]",
           &p.id, p.name, &p.age, p.disease) == 4) {
        printf("%-6d %-25s %-6d %s\n", p.id, p.name, p.age, p.disease);
        count++;
    }

    if (count == 0) printf("No records found!\n");
    else printf("------------------------------------------------------------\n");
    printf("Total Records: %d\n", count);

    fclose(fp);
}

/* ==================== SEARCH PATIENT ==================== */
void searchPatient() {
    FILE *fp;
    struct Patient p;
    int id, found = 0;

    fp = fopen("patients.txt", "r");
    if (fp == NULL) { printf("\nNo records found!\n"); return; }

    printf("\nEnter Patient ID to Search: ");
    if (!readInt(&id)) { fclose(fp); printf("Invalid ID!\n"); return; }

    while (fscanf(fp, "%d|%99[^|]|%d|%99[^\n]",
           &p.id, p.name, &p.age, p.disease) == 4) {
        if (p.id == id) {
            printf("\n--- Patient Found ---\n");
            printf("ID      : %d\n", p.id);
            printf("Name    : %s\n", p.name);
            printf("Age     : %d\n", p.age);
            printf("Disease : %s\n", p.disease);
            found = 1;
            break;
        }
    }

    if (!found) printf("\nPatient with ID %d not found!\n", id);
    fclose(fp);
}

/* ==================== UPDATE PATIENT ==================== */
void updatePatient() {
    FILE *fp, *temp;
    struct Patient p;
    int id, found = 0;
    char newName[100], newDisease[100];
    int newAge;

    fp = fopen("patients.txt", "r");
    if (fp == NULL) { printf("\nNo records found!\n"); return; }

    temp = fopen("temp.txt", "w");
    if (temp == NULL) {
        printf("Error: Cannot create temp file!\n");
        fclose(fp);
        return;
    }

    printf("\nEnter Patient ID to Update: ");
    if (!readInt(&id)) {
        fclose(fp); fclose(temp); remove("temp.txt");
        printf("Invalid ID!\n"); return;
    }

    
    /* First pass: check if patient exists */
    while (fscanf(fp, "%d|%99[^|]|%d|%99[^\n]",
           &p.id, p.name, &p.age, p.disease) == 4) {
        if (p.id == id) { found = 1; break; }
    }
    fclose(fp);
    fclose(temp);
    remove("temp.txt");

    if (!found) { printf("\nPatient with ID %d not found!\n", id); return; }

    /* Get new values */
    printf("Enter New Full Name  : ");
    if (!readLine(newName, sizeof(newName)) || strlen(newName) == 0) {
        printf("Invalid name!\n"); return;
    }

    printf("Enter New Age        : ");
    if (!readInt(&newAge)) { printf("Invalid age!\n"); return; }
    if (newAge <= 0 || newAge > 150) { printf("Error: Age must be 1-150!\n"); return; }

    printf("Enter New Disease    : ");
    if (!readLine(newDisease, sizeof(newDisease)) || strlen(newDisease) == 0) {
        printf("Invalid disease!\n"); return;
    }

    /* Second pass: rewrite file with updated record */
    fp = fopen("patients.txt", "r");
    temp = fopen("temp.txt", "w");
    if (fp == NULL || temp == NULL) {
        printf("Error: Cannot open files!\n");
        if (fp)   fclose(fp);
        if (temp) fclose(temp);
        return;
    }

    while (fscanf(fp, "%d|%99[^|]|%d|%99[^\n]",
           &p.id, p.name, &p.age, p.disease) == 4) {
        if (p.id == id) {
            fprintf(temp, "%d|%s|%d|%s\n", id, newName, newAge, newDisease);
        } else {
            fprintf(temp, "%d|%s|%d|%s\n", p.id, p.name, p.age, p.disease);
        }
    }

    fclose(fp);
    fclose(temp);
    remove("patients.txt");
    rename("temp.txt", "patients.txt");
    printf("\nPatient Updated Successfully!\n");
}

/* ==================== DELETE PATIENT ==================== */
void deletePatient() {
    FILE *fp, *temp;
    struct Patient p;
    int id, found = 0;

    fp = fopen("patients.txt", "r");
    if (fp == NULL) { printf("\nNo records found!\n"); return; }

    temp = fopen("temp.txt", "w");
    if (temp == NULL) {
        printf("Error: Cannot create temp file!\n");
        fclose(fp);
        return;
    }

    printf("\nEnter Patient ID to Delete: ");
    if (!readInt(&id)) {
        fclose(fp); fclose(temp); remove("temp.txt");
        printf("Invalid ID!\n"); return;
    }

    while (fscanf(fp, "%d|%99[^|]|%d|%99[^\n]",
           &p.id, p.name, &p.age, p.disease) == 4) {
        if (p.id == id) {
            found = 1;
            
        } else {
            fprintf(temp, "%d|%s|%d|%s\n", p.id, p.name, p.age, p.disease);
        }
    }

    fclose(fp);
    fclose(temp);
    remove("patients.txt");
    rename("temp.txt", "patients.txt");

    if (found) printf("\nPatient Deleted Successfully!\n");
    else       printf("\nPatient with ID %d not found!\n", id);
}
