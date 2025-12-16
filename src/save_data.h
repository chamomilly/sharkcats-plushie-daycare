#include <cstdio>

struct SaveData
{
    int selection;
    int happiness;
    int hunger;
    int energy;
};

void SaveGame(const SaveData &data)
{
    FILE *f = fopen("save.dat", "wb");
    if (!f)
        return;
    fwrite(&data, sizeof(SaveData), 1, f);
    fclose(f);
}

bool LoadGame(SaveData &data)
{
    FILE *f = fopen("save.dat", "rb");
    if (!f)
        return false;
    fread(&data, sizeof(SaveData), 1, f);
    fclose(f);
    return true;
}
