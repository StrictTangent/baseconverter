// converter.c
// Base Converter
// Paul Freeman 2020
//
// A small GTK application which performs
// base conversions between binary, decimal, & hexidecimal.

#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <math.h>
#include <ctype.h>

void bin_to_dec(char * result);
void dec_to_hex(char * result);
void hex_to_dec(char * result);
void dec_to_bin(char * result);

// Gtk Pointers //
GtkWidget * window;
GtkWidget * input_convert;
GtkWidget * label_result;
GtkWidget * button_convert;
GtkWidget * combo_types;
GtkWidget * combo_bitwidth;
GtkWidget * entry_types;
GtkWidget * entry_bitwidth;
GtkWidget * entry_result;

GtkBuilder * builder;

GtkListStore * liststore_types;
GtkListStore * liststore_bitwidth;

// Strings //
char input_string[128];
char conversion_type_string[80] = "Decimal -> Binary";

enum conversion_types {
    DEC_TO_BIN = 0,
    DEC_TO_HEX,
    BIN_TO_DEC,
    BIN_TO_HEX,
    HEX_TO_DEC,
    HEX_TO_BIN
};

enum conversion_types conversion_type = DEC_TO_BIN;
int bitwidth = 32;

int main (int argc, char * argv[]){

    gtk_init(&argc, &argv);

    builder = gtk_builder_new_from_resource("/converter/converter.glade");

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_builder_connect_signals(builder, NULL);

    input_convert = GTK_WIDGET(gtk_builder_get_object(builder, "input_to_convert"));
    label_result = GTK_WIDGET(gtk_builder_get_object(builder, "label_result"));
    button_convert = GTK_WIDGET(gtk_builder_get_object(builder, "button_convert"));
    combo_types = GTK_WIDGET(gtk_builder_get_object(builder, "combo_types"));
    combo_bitwidth = GTK_WIDGET(gtk_builder_get_object(builder, "combo_bitwidth"));
    entry_types = GTK_WIDGET(gtk_builder_get_object(builder, "entry_types"));
    entry_bitwidth = GTK_WIDGET(gtk_builder_get_object(builder, "entry_bitwidth"));
    entry_result = GTK_WIDGET(gtk_builder_get_object(builder, "entry_result"));

    liststore_types = GTK_LIST_STORE(gtk_builder_get_object(builder, "liststore_types"));
    liststore_bitwidth = GTK_LIST_STORE(gtk_builder_get_object(builder, "liststore_bitwidth"));

    gtk_label_set_text(GTK_LABEL(label_result), "Decimal -> Binary : 32 Bits");

    gtk_widget_show(window);
    gtk_main();
    return EXIT_SUCCESS;
}

// Update input_string when user enters value to convert
void on_input_to_convert_changed(GtkEntry *e){
    sprintf(input_string, "%s", gtk_entry_get_text(e));
}

// Update conversion_type and label when user changes
// conversion type in the combo box
void on_entry_types_changed(GtkEntry *e){

    // Set new label for conversion type
    char temp [128];
    strcpy(conversion_type_string, gtk_entry_get_text(e));
    sprintf(temp, "%s : %d Bits", conversion_type_string, bitwidth);
    gtk_label_set_text(GTK_LABEL(label_result), temp);
    
    // Update the value for conversion_type
    conversion_type = gtk_combo_box_get_active(GTK_COMBO_BOX(combo_types));
    
}

// Update bitwidth when user changes
// bitwidth in the combo box
void on_entry_bitwidth_changed(GtkEntry *e){
    char temp [128];
    bitwidth = atoi(gtk_entry_get_text(e));
    sprintf(temp, "%s : %d Bits", conversion_type_string, bitwidth);
    gtk_label_set_text(GTK_LABEL(label_result), temp);
}

// Perform the chosen conversion
void on_button_convert_clicked (GtkButton * b){
    char result[128];
    char temp[128];

    switch (conversion_type) {
        case DEC_TO_BIN:
            dec_to_bin(result);
            break;

        case DEC_TO_HEX:
            dec_to_hex(result);
            break;

        case BIN_TO_DEC:
            bin_to_dec(result);
            break;
        
        case BIN_TO_HEX:
            bin_to_dec(result);
            strcpy(temp, input_string);
            strcpy(input_string, result);
            dec_to_hex(result);
            strcpy(input_string, temp);
            break;

        case HEX_TO_DEC:
            hex_to_dec(result);
            break;

        case HEX_TO_BIN:
            hex_to_dec(result);
            strcpy(temp, input_string);
            strcpy(input_string, result);
            dec_to_bin(result);
            strcpy(input_string, temp);
    }

    gtk_entry_set_text(GTK_ENTRY(entry_result), result);

}

// User hits ENTER -> behave as if user hit convert button
void on_input_to_convert_activate (GtkEntry * e){
    on_button_convert_clicked(GTK_BUTTON(button_convert));
}

// Perform binary to decimal conversion
void bin_to_dec(char * result){
    long long int result_val = 0;
    long long int mask;
    int i;
    int len = strlen(input_string);

    for (i = len - 1; i >= len - bitwidth && i>= 0; i--){
        if (input_string[i] != '1' && input_string[i] != '0'){
            sprintf(result, "Not a valid binary representation");
            return;
        }
        mask = (input_string[i] == '1');
        result_val = result_val | (mask << ((len - 1) - i));
    }
    switch (bitwidth){
        case 8:
            sprintf(result, "%d", (int8_t) result_val);
            break;

        case 16:
            sprintf(result, "%d", (int16_t) result_val);
            break;

        case 32:
            sprintf(result, "%d", (int32_t) result_val);
            break;

        case 64:
            sprintf(result, "%ld", (int64_t) result_val);
            break;
    }
}

// Perform decimal to hexidecimal conversion
void dec_to_hex(char * result){
    int i = 0;
    while(input_string[i] != 0){

        if (i == 0 && input_string[i] == '-'){

        } else if (!(input_string[i] >= '0' && input_string[i] <= '9')){
            sprintf(result, "Not a valid decimal representation");
            return;
        }
        i++;
    }
    long long int value = atoll(input_string);
    fprintf(stderr, "value: %lld\n", value);
    switch(bitwidth){
        case 8:
            sprintf(result, "0x%02X",  (int8_t) value & 0xFF);
            break;
        case 16:
            sprintf(result, "0x%04X",  (int16_t) value & 0xFFFF);
            break;
        case 32: 
            sprintf(result, "0x%08X",  (int32_t) value & 0xFFFFFFFF);
            break;
        case 64:
            sprintf(result, "0x%lX",  (int64_t) value &  0xFFFFFFFFFFFFFFFF);
            break;
        }
}

// Perform hexidecimal to decimal conversion
void hex_to_dec(char * result){
    char * input_ptr = input_string;
    if (strstr(input_ptr, "0x") == input_string){
        fprintf(stderr, "advancing input pointer\n");
        input_ptr += 2;
    }
    long long int result_val = 0;
    char c;
    int i = 0;
    long long int nibble = 0;
    int found_significant = 1;

    while(input_ptr[i] != 0){
        c = input_ptr[i];
        if (c != '0' && found_significant == 0){
            found_significant = 1;
        } else if (found_significant) {
            result_val = result_val << 4;
        } else {
            i++;
            continue;
        }
        if (c >= '0' && c <= '9'){
            nibble = c - '0';
        } else if (c >= 'a' && c <= 'f'){
            nibble = c - 'a' + 10;
        } else if (c >= 'A' && c <= 'F'){
            nibble = c - 'A' + 10;
        } else {
            sprintf(result, "Not a valid hex representation");
            return;
        }
        fprintf(stderr,"nibble: %lld\n", nibble);
        result_val |= nibble;
        i++;
    }
    
    switch (bitwidth){
        case 8:
            sprintf(result, "%d", (int8_t) result_val);
            break;

        case 16:
            sprintf(result, "%d", (int16_t) result_val);
            break;

        case 32:
            sprintf(result, "%d", (int32_t) result_val);
            break;

        case 64:
            sprintf(result, "%ld", (int64_t) result_val);
            break;
    }
}

// Perform decimal to binary conversion
void dec_to_bin(char * result){
    int i = 0;
    while(input_string[i] != 0){

        if (i == 0 && input_string[i] == '-'){

        } else if (!(input_string[i] >= '0' && input_string[i] <= '9')){
            sprintf(result, "Not a valid decimal representation");
            return;
        }
        i++;
    }
    long long int value = atoll(input_string);
    long long int mask = 1;
    result[bitwidth] = 0;
    for (i = 0; i < bitwidth; i++){
        if (mask<<(bitwidth - i - 1) & value){
            result[i] = '1';
        } else {
            result[i] = '0';
        }
    }
}