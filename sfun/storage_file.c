#include "storage_file.h"

static FILE * _fp = NULL;
static uint32_t _num_rows = 0;
static uint32_t _num_cols = 0;

bool storage_exists(const char * name) {
    _fp = fopen(name, "r");

    if (!_fp) {
        return false;
    } else {
        fclose(_fp);
        return true;
    }
}

bool storage_open_write(const char * name) {
    if (_fp) {
        fclose(_fp);
        _fp = NULL;
    }

    _fp = fopen(name, "wb");

    if (!_fp) {
        return false;
    } else {
        return true;
    }
}

bool storage_open_read(const char * name) {
    if (_fp) {
        fclose(_fp);
        _fp = NULL;
    }

    _fp = fopen(name, "rb");

    if (!_fp) {
        return false;
    } else {
        return true;
    }
}

bool storage_save(uint32_t num_rows, uint32_t num_cols, double ** data) {
    if (_fp) {
        uint32_t n;

        if (1 != fwrite(&num_rows, sizeof(uint32_t), 1, _fp)) {
            return false;
        }

        if (1 != fwrite(&num_cols, sizeof(uint32_t), 1, _fp)) {
            return false;
        }
        for (n = 0; n < num_rows; n++) {
            if (num_cols != fwrite(data[n], sizeof(double), num_cols, _fp)) {
                return false;
            }
        }
        return true;
    } else {
        return false;
    }
}

void storage_close(void) {
    if (_fp) {
        fclose(_fp);
        _fp = NULL;
    }
}

bool storage_load_header(uint32_t * num_rows, uint32_t * num_cols) {
    if (!_fp) {
        return false;
    }

    if (fread(num_rows, sizeof(unsigned int), 1, _fp) != 1) {
        return false;
    }

    _num_rows = *num_rows;

    if (fread(num_cols, sizeof(unsigned int), 1, _fp) != 1) {
        return false;
    }

    _num_cols = *num_cols;

    return true;
}

bool storage_load_data(double ** data) {
    if (_fp) {
        uint32_t i, j;
        for (i = 0; i < _num_rows; i++) {
            for (j = 0; j < _num_cols; j++) {
                double d = 0;
                if (1 != fread(&d, sizeof(double), 1, _fp)) {
                    return false;
                } else {
                    data[i][j] = d;
                }
            }
        }
        return true;
    } else {
        return false;
    }
}
