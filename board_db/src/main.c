/*/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * main.c
 * Copyright (C) 2024 alex <alex@alex-RLEF-XX>
 * 
 * board_db is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * board_db is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>
#include <gtk/gtk.h>

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

#include <glib/gi18n.h>

#define MAX_GEOPOINT_COUNT 100     
#define MAX_AERDRM_COUNT   110     
#define MAX_ROUTES_COUNT   120   
#define MAX_RM_RSBN_COUNT  130     
#define MAX_RM_VOR_COUNT   140     
#define MAX_ZONE_COUNT 	   150     
#define POINT_NAME_SIZE    256

//enum type_db { GEO_DB, AER_DB, MRSH_DB, RSBN_DB, VOR_DB, ZONE_ZAP_DB, ZONE_PLT_DB, ZONE_AER_DB, aer_db };
enum type_db { geo_db, aer_db, mrsh_db, rsbn_db, vor_db, zone_zap_db, zone_plt_db, zone_aer_db };

typedef struct _Private Private;
struct _Private
{
	/* ANJUTA: Widgets declaration for board_db.ui - DO NOT REMOVE */
};

static Private* priv = NULL;

/* For testing purpose, define TEST to use the local (not installed) ui file */
#define TEST
#ifdef TEST
#define UI_FILE "src/board_db.ui"
#else
#define UI_FILE PACKAGE_DATA_DIR"/ui/board_db.ui"
#endif
#define TOP_WINDOW "window"

/* Signal handlers */
/* Note: These may not be declared static because signal autoconnection
 * only works with non-static methods
 */

/* Called when the window is closed */
void
on_window_destroy (GtkWidget *widget, gpointer data)
{
	gtk_main_quit ();
}

static GtkWidget*
create_window (void)
{
	GtkWidget *window;
	GtkBuilder *builder;
	GError* error = NULL;

	/* Load UI from file */
	builder = gtk_builder_new ();
	if (!gtk_builder_add_from_file (builder, UI_FILE, &error))
	{
		g_critical ("Couldn't load builder file: %s", error->message);
		g_error_free (error);
	}

	/* Auto-connect signal handlers */
	gtk_builder_connect_signals (builder, NULL);

	/* Get the window object from the ui file */
	window = GTK_WIDGET (gtk_builder_get_object (builder, TOP_WINDOW));
        if (!window)
        {
                g_critical ("Widget \"%s\" is missing in file %s.",
				TOP_WINDOW,
				UI_FILE);
        }

	priv = g_malloc (sizeof (struct _Private));
	/* ANJUTA: Widgets initialization for board_db.ui - DO NOT REMOVE */

	g_object_unref (builder);

	
	return window;
}


/* Геоточки */
typedef struct s_geo_add
{
	uint num;
	char name[ POINT_NAME_SIZE ];
	double lat;
	double lon;
	float hAbs;
	float declination;

	// Валидности ( 0 - не валидно | 1 - валидно )
	struct 
	{
		uint num 			: 1; //!< валидность порядкового номера геоточки
		uint name 			: 1; //!< валидность названия геоточки
		uint lat 			: 1; //!< валидность широты
		uint lon 			: 1; //!< валидность долготы
		uint hAbs 			: 1; //!< валидность абсолютной высоты
		uint declination 	: 1; //!< валидность магнитного склонения
		uint editability 	: 1; //!< возможность редактирования
		uint deletable 		: 1; //!< возможность удалить. Является частью маршрута
									 //!< ( 0 - является/нельзя удалить | 1 - не является/можно удалить ) 
		
		uint reserve 		: (32-8); //!< резерв
	} valid;
	
} s_geo_add_t;
		
/* Элемент геоточек */
typedef struct s_geo_db
{
	s_geo_add_t geo_add;
	// uint32_t count;
} s_geo_db_t;

/* Аэродромы */
typedef struct s_aer_add
{
	uint num;
	char name[ POINT_NAME_SIZE ];
	double lat;
	double lon;
	float hAbs;
	float declination;

	// Валидности ( 0 - не валидно | 1 - валидно )
	struct 
	{
		uint num 			: 1; //!< валидность порядкового номера геоточки
		uint name 			: 1; //!< валидность названия геоточки
		uint lat 			: 1; //!< валидность широты
		uint lon 			: 1; //!< валидность долготы
		uint hAbs 			: 1; //!< валидность абсолютной высоты
		uint declination 	: 1; //!< валидность магнитного склонения
		uint editability 	: 1; //!< возможность редактирования
		uint deletable 		: 1; //!< возможность удалить. Является частью маршрута
									 //!< ( 0 - является/нельзя удалить | 1 - не является/можно удалить ) 
		
		uint reserve 		: (32-8); //!< резерв
	} valid;
	
} s_aer_add_t;

/* Элементы точек аэродрома */
typedef struct s_aer_db
{
	s_aer_add_t aer_add;
	// uint32_t count;
} s_aer_db_t;

/* Маршрутные точки */
typedef struct s_mrsh_add
{
	uint num;
	char name[ POINT_NAME_SIZE ];
	double lat;
	double lon;
	float hAbs;
	float declination;

	// Валидности ( 0 - не валидно | 1 - валидно )
	struct 
	{
		uint num 			: 1; //!< валидность порядкового номера геоточки
		uint name 			: 1; //!< валидность названия геоточки
		uint lat 			: 1; //!< валидность широты
		uint lon 			: 1; //!< валидность долготы
		uint hAbs 			: 1; //!< валидность абсолютной высоты
		uint declination 	: 1; //!< валидность магнитного склонения
		uint editability 	: 1; //!< возможность редактирования
		uint deletable 		: 1; //!< возможность удалить. Является частью маршрута
									 //!< ( 0 - является/нельзя удалить | 1 - не является/можно удалить ) 
		
		uint reserve 		: (32-8); //!< резерв
	} valid;
	
} s_route_add_t;

/* Элементы точек маршрута */
typedef struct s_mrsh_db
{
	s_route_add_t mrsh_add;
	// uint32_t count;
} s_route_db_t;

/* РСБН точки */
typedef struct s_rsbn_add
{
	uint num;
	char name[ POINT_NAME_SIZE ];
	double lat;
	double lon;
	float hAbs;
	float declination;

	// Валидности ( 0 - не валидно | 1 - валидно )
	struct 
	{
		uint num 			: 1; //!< валидность порядкового номера геоточки
		uint name 			: 1; //!< валидность названия геоточки
		uint lat 			: 1; //!< валидность широты
		uint lon 			: 1; //!< валидность долготы
		uint hAbs 			: 1; //!< валидность абсолютной высоты
		uint declination 	: 1; //!< валидность магнитного склонения
		uint editability 	: 1; //!< возможность редактирования
		uint deletable 		: 1; //!< возможность удалить. Является частью маршрута
									 //!< ( 0 - является/нельзя удалить | 1 - не является/можно удалить ) 
		
		uint reserve 		: (32-8); //!< резерв
	} valid;
	
} s_rsbn_add_t;

/* Элементы точек РСБН */
typedef struct s_rsbn_db
{
	s_rsbn_add_t rsbn_add;
	// uint32_t count;
} s_rsbn_db_t;


/* VOR точки */
typedef struct s_vor_add
{
	uint num;
	char name[ POINT_NAME_SIZE ];
	double lat;
	double lon;
	float hAbs;
	float declination;

	// Валидности ( 0 - не валидно | 1 - валидно )
	struct 
	{
		uint num 			: 1; //!< валидность порядкового номера геоточки
		uint name 			: 1; //!< валидность названия геоточки
		uint lat 			: 1; //!< валидность широты
		uint lon 			: 1; //!< валидность долготы
		uint hAbs 			: 1; //!< валидность абсолютной высоты
		uint declination 	: 1; //!< валидность магнитного склонения
		uint editability 	: 1; //!< возможность редактирования
		uint deletable 		: 1; //!< возможность удалить. Является частью маршрута
									 //!< ( 0 - является/нельзя удалить | 1 - не является/можно удалить ) 
		
		uint reserve 		: (32-8); //!< резерв
	} valid;
	
} s_vor_add_t;

/* Элементы точек VOR */
typedef struct s_vor_db
{
	s_vor_add_t vor_add;
	// uint32_t count;
} s_vor_db_t;

/* ZONE ZAP точки */
typedef struct s_zone_zap_add
{
	uint num;
	char name[ POINT_NAME_SIZE ];
	double lat;
	double lon;
	float hAbs;
	float declination;

	// Валидности ( 0 - не валидно | 1 - валидно )
	struct 
	{
		uint num 			: 1; //!< валидность порядкового номера геоточки
		uint name 			: 1; //!< валидность названия геоточки
		uint lat 			: 1; //!< валидность широты
		uint lon 			: 1; //!< валидность долготы
		uint hAbs 			: 1; //!< валидность абсолютной высоты
		uint declination 	: 1; //!< валидность магнитного склонения
		uint editability 	: 1; //!< возможность редактирования
		uint deletable 		: 1; //!< возможность удалить. Является частью маршрута
									 //!< ( 0 - является/нельзя удалить | 1 - не является/можно удалить ) 
		
		uint reserve 		: (32-8); //!< резерв
	} valid;
	
} s_zone_zap_add_t;

/* Элементы точек ZONE ZAP */
typedef struct s_zone_zap_db
{
	s_zone_zap_add_t zone_zap_add;
	// uint32_t count;
} s_zone_zap_db_t;

/* ZONE PIL точки */
typedef struct s_zone_plt_add
{
	uint num;
	char name[ POINT_NAME_SIZE ];
	double lat;
	double lon;
	float hAbs;
	float declination;

	// Валидности ( 0 - не валидно | 1 - валидно )
	struct 
	{
		uint num 			: 1; //!< валидность порядкового номера геоточки
		uint name 			: 1; //!< валидность названия геоточки
		uint lat 			: 1; //!< валидность широты
		uint lon 			: 1; //!< валидность долготы
		uint hAbs 			: 1; //!< валидность абсолютной высоты
		uint declination 	: 1; //!< валидность магнитного склонения
		uint editability 	: 1; //!< возможность редактирования
		uint deletable 		: 1; //!< возможность удалить. Является частью маршрута
									 //!< ( 0 - является/нельзя удалить | 1 - не является/можно удалить ) 
		
		uint reserve 		: (32-8); //!< резерв
	} valid;
	
} s_zone_pil_add_t;

/* Элементы точек ZONE PIL */
typedef struct s_zone_plt_db
{
	s_zone_pil_add_t zone_plt_add;
	// uint32_t count;
} s_zone_pil_db_t;

/* ZONE AER точки */
typedef struct s_zone_aer_add
{
	uint num;
	char name[ POINT_NAME_SIZE ];
	double lat;
	double lon;
	float hAbs;
	float declination;

	// Валидности ( 0 - не валидно | 1 - валидно )
	struct 
	{
		uint num 			: 1; //!< валидность порядкового номера геоточки
		uint name 			: 1; //!< валидность названия геоточки
		uint lat 			: 1; //!< валидность широты
		uint lon 			: 1; //!< валидность долготы
		uint hAbs 			: 1; //!< валидность абсолютной высоты
		uint declination 	: 1; //!< валидность магнитного склонения
		uint editability 	: 1; //!< возможность редактирования
		uint deletable 		: 1; //!< возможность удалить. Является частью маршрута
									 //!< ( 0 - является/нельзя удалить | 1 - не является/можно удалить ) 
		
		uint reserve 		: (32-8); //!< резерв
	} valid;
	
} s_zone_aer_add_t;

/* Элементы точек ZONE AER */
typedef struct s_zone_aer_db
{
	s_zone_aer_add_t zone_aer_add;
	// uint32_t count;
} s_zone_aer_db_t;


/* Структура дополнительной БД */
typedef struct s_board_db
{
	s_geo_db_t		geo_db 		[ MAX_GEOPOINT_COUNT ];
	s_aer_db_t		aer_db 		[ MAX_AERDRM_COUNT 	 ];
	s_route_db_t	mrsh_db 	[ MAX_ROUTES_COUNT 	 ];
	s_rsbn_db_t		rsbn_db 	[ MAX_RM_RSBN_COUNT  ];
	s_vor_db_t		vor_db 		[ MAX_RM_VOR_COUNT   ];
	s_zone_zap_db_t	zone_zap_db [ MAX_ZONE_COUNT 	 ];
	s_zone_pil_db_t	zone_plt_db [ MAX_ZONE_COUNT 	 ];
	s_zone_aer_db_t	zone_aer_db [ MAX_ZONE_COUNT 	 ];
} s_board_db_t;

/*
Что должна делать программа:
- считывать из бинарного файла ББД массив геоточек
- выводить в отдельной форме для геоточки с номером N её параметры  (поля)
- позволять добавлять / редактировать / удалять геоточки
- сохранять новый файл ББД
имя файла: db.dat
кодировка - win1251
добавить валидность справа
номер, имя, долгота и широта - обязательные поля. Если человек не вводит высоту и склонение, то ничего страшного.
Если человек не вводит имя или долготу или широту или номер, то не давать программе запуститься
 */


/* Считывание из бинарного файла ББД массив геоточек */
void 
read_board_db( GtkWidget *widget, gpointer data, int size_board_db, char warning[], enum type_db current )
{
	s_board_db_t *board_db = ( s_board_db_t * )data;
	
	const char* filename = "db.dat";
	FILE* file = fopen( filename, "rb, ccs=windows-1251" );

	if ( !file ) {
		printf( "Ошибка открытия файла %s !\n", filename );
		return;
	}

	
	int num_points = 0;
	while ( num_points < ( MAX_GEOPOINT_COUNT + MAX_AERDRM_COUNT +
			                      MAX_ROUTES_COUNT + MAX_RM_RSBN_COUNT +
			                      MAX_RM_VOR_COUNT + MAX_ZONE_COUNT * 3 ) ) 
	{
		s_board_db_t point;
		if ( fread( &point, sizeof( s_board_db_t ), 2, file ) != 1 ) 
		{ 
			break;
		}
		board_db[ num_points++ ] = point;
	}
	printf( "Прочитано\n" );
	
    GtkWidget *confirm_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_OTHER, GTK_BUTTONS_OK, "OK" );
    gtk_dialog_run( GTK_DIALOG( confirm_dialog ) );
    gtk_widget_destroy( confirm_dialog );
	
	// Проверка
	int i = 0;
	printf("%s [%d]\n", warning, size_board_db);
	for( i = 0; i < size_board_db; ++i ) {
		switch ( current ) {
			case geo_db: {
				printf("\nНомер: %u\n Имя: %s\n Широта: %lf\n Долгота: %lf\n Абсолютная высота: %f\n Склонение: %f\n", 
					   board_db->geo_db[ i ].geo_add.num,	
					   board_db->geo_db[ i ].geo_add.name,	
					   board_db->geo_db[ i ].geo_add.lat,
					   board_db->geo_db[ i ].geo_add.lon,
					   board_db->geo_db[ i ].geo_add.hAbs,
					   board_db->geo_db[ i ].geo_add.declination);
				printf("-------------------------------");
				break;
			}
			case aer_db: {
				printf("\nНомер: %u\n Имя: %s\n Широта: %lf\n Долгота: %lf\n Абсолютная высота: %f\n Склонение: %f\n", 
					   board_db->aer_db[ i ].aer_add.num,	
					   board_db->aer_db[ i ].aer_add.name,	
					   board_db->aer_db[ i ].aer_add.lat,
					   board_db->aer_db[ i ].aer_add.lon,
					   board_db->aer_db[ i ].aer_add.hAbs,
					   board_db->aer_db[ i ].aer_add.declination);
				printf("-------------------------------");
				break;
			}
			case rsbn_db: {
				printf("\nНомер: %u\n Имя: %s\n Широта: %lf\n Долгота: %lf\n Абсолютная высота: %f\n Склонение: %f\n", 
					   board_db->rsbn_db[ i ].rsbn_add.num,	
					   board_db->rsbn_db[ i ].rsbn_add.name,	
					   board_db->rsbn_db[ i ].rsbn_add.lat,
					   board_db->rsbn_db[ i ].rsbn_add.lon,
					   board_db->rsbn_db[ i ].rsbn_add.hAbs,
					   board_db->rsbn_db[ i ].rsbn_add.declination);
				printf("-------------------------------");
				break;
			}
			case vor_db: {
				printf("\nНомер: %u\n Имя: %s\n Широта: %lf\n Долгота: %lf\n Абсолютная высота: %f\n Склонение: %f\n", 
					   board_db->vor_db[ i ].vor_add.num,	
					   board_db->vor_db[ i ].vor_add.name,	
					   board_db->vor_db[ i ].vor_add.lat,
					   board_db->vor_db[ i ].vor_add.lon,
					   board_db->vor_db[ i ].vor_add.hAbs,
					   board_db->vor_db[ i ].vor_add.declination);
				printf("-------------------------------");
				break;
			}
			case mrsh_db: {
				printf("\nНомер: %u\n Имя: %s\n Широта: %lf\n Долгота: %lf\n Абсолютная высота: %f\n Склонение: %f\n", 
					   board_db->mrsh_db[ i ].mrsh_add.num,	
					   board_db->mrsh_db[ i ].mrsh_add.name,	
					   board_db->mrsh_db[ i ].mrsh_add.lat,
					   board_db->mrsh_db[ i ].mrsh_add.lon,
					   board_db->mrsh_db[ i ].mrsh_add.hAbs,
					   board_db->mrsh_db[ i ].mrsh_add.declination);
				printf("-------------------------------");
				break;
			}
			case zone_zap_db: {
				printf("\nНомер: %u\n Имя: %s\n Широта: %lf\n Долгота: %lf\n Абсолютная высота: %f\n Склонение: %f\n", 
					   board_db->zone_zap_db[ i ].zone_zap_add.num,	
					   board_db->zone_zap_db[ i ].zone_zap_add.name,	
					   board_db->zone_zap_db[ i ].zone_zap_add.lat,
					   board_db->zone_zap_db[ i ].zone_zap_add.lon,
					   board_db->zone_zap_db[ i ].zone_zap_add.hAbs,
					   board_db->zone_zap_db[ i ].zone_zap_add.declination);
				printf("-------------------------------");
				break;
			}
			case zone_plt_db: {
				printf("\nНомер: %u\n Имя: %s\n Широта: %lf\n Долгота: %lf\n Абсолютная высота: %f\n Склонение: %f\n", 
					   board_db->zone_plt_db[ i ].zone_plt_add.num,	
					   board_db->zone_plt_db[ i ].zone_plt_add.name,	
					   board_db->zone_plt_db[ i ].zone_plt_add.lat,
					   board_db->zone_plt_db[ i ].zone_plt_add.lon,
					   board_db->zone_plt_db[ i ].zone_plt_add.hAbs,
					   board_db->zone_plt_db[ i ].zone_plt_add.declination);
				printf("-------------------------------");
				break;
			}
			case zone_aer_db: {
				printf("\nНомер: %u\n Имя: %s\n Широта: %lf\n Долгота: %lf\n Абсолютная высота: %f\n Склонение: %f\n", 
					   board_db->zone_aer_db[ i ].zone_aer_add.num,	
					   board_db->zone_aer_db[ i ].zone_aer_add.name,	
					   board_db->zone_aer_db[ i ].zone_aer_add.lat,
					   board_db->zone_aer_db[ i ].zone_aer_add.lon,
					   board_db->zone_aer_db[ i ].zone_aer_add.hAbs,
					   board_db->zone_aer_db[ i ].zone_aer_add.declination);
				printf("-------------------------------");
				break;
			}
			default:
				break;

		}
		
	}
	fclose( file );
}


/* Запись в бинарный файл массив геоточек */
void 
write_board_db( GtkWidget *widget, gpointer data )
{
	s_board_db_t *board_db  = ( s_board_db_t * )data;
	
	const char* filename = "db.dat";
	FILE* file = fopen( filename, "wb, ccs=windows-1251" );

	if ( !file ) {
		printf( "Ошибка открытия файла %s !\n", filename );
		return;
	}	

	fwrite( board_db, sizeof(s_board_db_t), 1, file );
	printf("\nСохранено\n" );
	
	GtkWidget *confirm_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_OTHER, GTK_BUTTONS_OK, "OK" );
    gtk_dialog_run( GTK_DIALOG( confirm_dialog ) );
    gtk_widget_destroy( confirm_dialog );
	
	fclose( file );
}


/* Проверяем валидность данных. Коррекция данных */
/* Translate: Check validate. Correction of data */
void check_valid( s_board_db_t *board_db, enum type_db current ){
	int i = 0;
	switch ( current ) {
		case geo_db: {
			while ( i < MAX_GEOPOINT_COUNT )
			{
					if ( board_db->geo_db[ i ].geo_add.valid.declination == 0 	||
					board_db->geo_db[ i ].geo_add.valid.num == 0 				||
					board_db->geo_db[ i ].geo_add.valid.name == 0				||
					board_db->geo_db[ i ].geo_add.valid.lat == 0 				||
					board_db->geo_db[ i ].geo_add.valid.lon == 0 				||
					board_db->geo_db[ i ].geo_add.valid.hAbs == 0 )
				{
					board_db->geo_db[ i ].geo_add.num 			= 0;
					strncpy( board_db->geo_db[ i ].geo_add.name, "", POINT_NAME_SIZE - 1 );
					board_db->geo_db[ i ].geo_add.lat 			= 0.0;
					board_db->geo_db[ i ].geo_add.lon 			= 0.0;
					board_db->geo_db[ i ].geo_add.hAbs 			= 0.0;
					board_db->geo_db[ i ].geo_add.declination 	= 0.0;
				}
				++i;
			}
			return;
		}
		case aer_db: {
			while ( i < MAX_AERDRM_COUNT )
			{
					if ( board_db->aer_db[ i ].aer_add.valid.declination == 0 	||
					board_db->aer_db[ i ].aer_add.valid.num == 0 				||
					board_db->aer_db[ i ].aer_add.valid.name == 0 				||
					board_db->aer_db[ i ].aer_add.valid.lat == 0 				||
					board_db->aer_db[ i ].aer_add.valid.lon == 0 				||
					board_db->aer_db[ i ].aer_add.valid.hAbs == 0 )
				{
					board_db->aer_db[ i ].aer_add.num 			= 0;
					strncpy( board_db->aer_db[ i ].aer_add.name, "", POINT_NAME_SIZE - 1 );
					board_db->aer_db[ i ].aer_add.lat 			= 0.0;
					board_db->aer_db[ i ].aer_add.lon 			= 0.0;
					board_db->aer_db[ i ].aer_add.hAbs 			= 0.0;
					board_db->aer_db[ i ].aer_add.declination 	= 0.0;
				}
				++i;
			}
			return;
		}
		case mrsh_db: {
			while ( i < MAX_ROUTES_COUNT )
			{
					if ( board_db->mrsh_db[ i ].mrsh_add.valid.declination == 0 ||
					board_db->mrsh_db[ i ].mrsh_add.valid.num == 0 				||
					board_db->mrsh_db[ i ].mrsh_add.valid.name == 0 			||
					board_db->mrsh_db[ i ].mrsh_add.valid.lat == 0 				||
					board_db->mrsh_db[ i ].mrsh_add.valid.lon == 0 				||
					board_db->mrsh_db[ i ].mrsh_add.valid.hAbs == 0 )
				{
					board_db->mrsh_db[ i ].mrsh_add.num 		= 0;
					strncpy( board_db->mrsh_db[ i ].mrsh_add.name, "", POINT_NAME_SIZE - 1 );
					board_db->mrsh_db[ i ].mrsh_add.lat 		= 0.0;
					board_db->mrsh_db[ i ].mrsh_add.lon 		= 0.0;
					board_db->mrsh_db[ i ].mrsh_add.hAbs 		= 0.0;
					board_db->mrsh_db[ i ].mrsh_add.declination = 0.0;
				}
				++i;
			}
			return;
		}
		case vor_db: {
			while ( i < MAX_RM_VOR_COUNT )
			{
					if ( board_db->vor_db[ i ].vor_add.valid.declination == 0 	||
					board_db->vor_db[ i ].vor_add.valid.num == 0 				||
					board_db->vor_db[ i ].vor_add.valid.name == 0 				||
					board_db->vor_db[ i ].vor_add.valid.lat == 0 				||
					board_db->vor_db[ i ].vor_add.valid.lon == 0 				||
					board_db->vor_db[ i ].vor_add.valid.hAbs == 0 )
				{
					board_db->vor_db[ i ].vor_add.num 			= 0;
					strncpy( board_db->vor_db[ i ].vor_add.name, "", POINT_NAME_SIZE - 1 );
					board_db->vor_db[ i ].vor_add.lat 			= 0.0;
					board_db->vor_db[ i ].vor_add.lon 			= 0.0;
					board_db->vor_db[ i ].vor_add.hAbs 			= 0.0;
					board_db->vor_db[ i ].vor_add.declination 	= 0.0;
				}
				++i;
			}
			return;
		}
		case zone_zap_db: {
			while ( i < MAX_ZONE_COUNT )
			{
					if ( board_db->zone_zap_db[ i ].zone_zap_add.valid.declination == 0 ||
					board_db->zone_zap_db[ i ].zone_zap_add.valid.num == 0 				||
					board_db->zone_zap_db[ i ].zone_zap_add.valid.name == 0 			||
					board_db->zone_zap_db[ i ].zone_zap_add.valid.lat == 0 				||
					board_db->zone_zap_db[ i ].zone_zap_add.valid.lon == 0 				||
					board_db->zone_zap_db[ i ].zone_zap_add.valid.hAbs == 0 )
				{
					board_db->zone_zap_db[ i ].zone_zap_add.num 		= 0;
					strncpy( board_db->zone_zap_db[ i ].zone_zap_add.name, "", POINT_NAME_SIZE - 1 );
					board_db->zone_zap_db[ i ].zone_zap_add.lat 		= 0.0;
					board_db->zone_zap_db[ i ].zone_zap_add.lon 		= 0.0;
					board_db->zone_zap_db[ i ].zone_zap_add.hAbs 		= 0.0;
					board_db->zone_zap_db[ i ].zone_zap_add.declination = 0.0;
				}
				++i;
			}
			return;
		}
		case zone_plt_db: {
			while ( i < MAX_ZONE_COUNT )
			{
					if ( board_db->zone_plt_db[ i ].zone_plt_add.valid.declination == 0 ||
					board_db->zone_plt_db[ i ].zone_plt_add.valid.num == 0 				||
					board_db->zone_plt_db[ i ].zone_plt_add.valid.name == 0 			||
					board_db->zone_plt_db[ i ].zone_plt_add.valid.lat == 0 				||
					board_db->zone_plt_db[ i ].zone_plt_add.valid.lon == 0 				||
					board_db->zone_plt_db[ i ].zone_plt_add.valid.hAbs == 0 )
				{
					board_db->zone_plt_db[ i ].zone_plt_add.num 		= 0;
					strncpy( board_db->zone_plt_db[ i ].zone_plt_add.name, "", POINT_NAME_SIZE - 1 );
					board_db->zone_plt_db[ i ].zone_plt_add.lat 		= 0.0;
					board_db->zone_plt_db[ i ].zone_plt_add.lon 		= 0.0;
					board_db->zone_plt_db[ i ].zone_plt_add.hAbs 		= 0.0;
					board_db->zone_plt_db[ i ].zone_plt_add.declination = 0.0;
				}
				++i;
			}
			return;
		}
		case zone_aer_db: {
			while ( i < MAX_ZONE_COUNT )
			{
					if ( board_db->zone_aer_db[ i ].zone_aer_add.valid.declination == 0 ||
					board_db->zone_aer_db[ i ].zone_aer_add.valid.num == 0 				||
					board_db->zone_aer_db[ i ].zone_aer_add.valid.name == 0 			||
					board_db->zone_aer_db[ i ].zone_aer_add.valid.lat == 0 				||
					board_db->zone_aer_db[ i ].zone_aer_add.valid.lon == 0 				||
					board_db->zone_aer_db[ i ].zone_aer_add.valid.hAbs == 0 )
				{
					board_db->zone_aer_db[ i ].zone_aer_add.num 		= 0;
					strncpy( board_db->zone_aer_db[ i ].zone_aer_add.name, "", POINT_NAME_SIZE - 1 );
					board_db->zone_aer_db[ i ].zone_aer_add.lat 		= 0.0;
					board_db->zone_aer_db[ i ].zone_aer_add.lon 		= 0.0;
					board_db->zone_aer_db[ i ].zone_aer_add.hAbs 		= 0.0;
					board_db->zone_aer_db[ i ].zone_aer_add.declination = 0.0;
				}
				++i;
			}
			return;
		}
		case rsbn_db: {
			while ( i < MAX_RM_RSBN_COUNT )
			{
					if ( board_db->rsbn_db[ i ].rsbn_add.valid.declination == 0 ||
					board_db->rsbn_db[ i ].rsbn_add.valid.num == 0 				||
					board_db->rsbn_db[ i ].rsbn_add.valid.name == 0 			||
					board_db->rsbn_db[ i ].rsbn_add.valid.lat == 0 				||
					board_db->rsbn_db[ i ].rsbn_add.valid.lon == 0 				||
					board_db->rsbn_db[ i ].rsbn_add.valid.hAbs == 0 )
				{
					board_db->rsbn_db[ i ].rsbn_add.num 		= 0;
					strncpy( board_db->rsbn_db[ i ].rsbn_add.name, "", POINT_NAME_SIZE - 1 );
					board_db->rsbn_db[ i ].rsbn_add.lat 		= 0.0;
					board_db->rsbn_db[ i ].rsbn_add.lon 		= 0.0;
					board_db->rsbn_db[ i ].rsbn_add.hAbs 		= 0.0;
					board_db->rsbn_db[ i ].rsbn_add.declination = 0.0;
				}
				++i;
			}
			return;
		}
		default:
			return;
	}
}


/* Автоматически дописываем номер */
/* Translate: automatic write num */
void write_number( s_board_db_t* board_db, char* str_num, int size_board_db, enum type_db current ) 
{
switch ( current ) {
	case geo_db: {
		int j;
		for ( j = 0; j < MAX_GEOPOINT_COUNT; ++j )
		{
			if ( board_db->geo_db[ j ].geo_add.num == 0 )
				break;
		}
		if ( board_db->geo_db[ 0 ].geo_add.num == 0 ) 
			sprintf(str_num, "%u", 1 );

		else 
			sprintf(str_num, "%u", board_db->geo_db[ j-1 ].geo_add.num + 1 );
		return;
	}
	case aer_db: {
		int j;
		for ( j = 0; j < MAX_AERDRM_COUNT; ++j )
		{
			if ( board_db->aer_db[ j ].aer_add.num == 0 )
				break;
		}
		if ( board_db->aer_db[ 0 ].aer_add.num == 0 ) 
			sprintf(str_num, "%u", 1 );

		else 
			sprintf(str_num, "%u", board_db->aer_db[ j-1 ].aer_add.num + 1 );
		return;
	}
	case mrsh_db: {
		int j;
		for ( j = 0; j < MAX_ROUTES_COUNT; ++j )
		{
			if ( board_db->mrsh_db[ j ].mrsh_add.num == 0 )
				break;
		}
		if ( board_db->mrsh_db[ 0 ].mrsh_add.num == 0 ) 
			sprintf(str_num, "%u", 1 );

		else 
			sprintf(str_num, "%u", board_db->mrsh_db[ j-1 ].mrsh_add.num + 1 );
		return;
	}
	case rsbn_db: {
		int j;
		for ( j = 0; j < MAX_RM_RSBN_COUNT; ++j )
		{
			if ( board_db->rsbn_db[ j ].rsbn_add.num == 0 )
				break;
		}
		if ( board_db->rsbn_db[ 0 ].rsbn_add.num == 0 ) 
			sprintf(str_num, "%u", 1 );

		else 
			sprintf(str_num, "%u", board_db->rsbn_db[ j-1 ].rsbn_add.num + 1 );
		return;
	}
	case vor_db: {
		int j;
		for ( j = 0; j < MAX_RM_VOR_COUNT; ++j )
		{
			if ( board_db->vor_db[ j ].vor_add.num == 0 )
				break;
		}
		if ( board_db->vor_db[ 0 ].vor_add.num == 0 ) 
			sprintf(str_num, "%u", 1 );

		else 
			sprintf(str_num, "%u", board_db->vor_db[ j-1 ].vor_add.num + 1 );
		return;
	}
	case zone_zap_db: {
		int j;
		for ( j = 0; j < MAX_ZONE_COUNT; ++j )
		{
			if ( board_db->zone_zap_db[ j ].zone_zap_add.num == 0 )
				break;
		}
		if ( board_db->zone_zap_db[ 0 ].zone_zap_add.num == 0 ) 
			sprintf(str_num, "%u", 1 );

		else 
			sprintf(str_num, "%u", board_db->zone_zap_db[ j-1 ].zone_zap_add.num + 1 );
		return;
	}
	case zone_plt_db: {
		int j;
		for ( j = 0; j < MAX_ZONE_COUNT; ++j )
		{
			if ( board_db->zone_plt_db[ j ].zone_plt_add.num == 0 )
				break;
		}
		if ( board_db->zone_plt_db[ 0 ].zone_plt_add.num == 0 ) 
			sprintf(str_num, "%u", 1 );

		else 
			sprintf(str_num, "%u", board_db->zone_plt_db[ j-1 ].zone_plt_add.num + 1 );
		return;
	}
	case zone_aer_db: {
		int j;
		for ( j = 0; j < MAX_ZONE_COUNT; ++j )
		{
			if ( board_db->zone_aer_db[ j ].zone_aer_add.num == 0 )
				break;
		}
		if ( board_db->zone_aer_db[ 0 ].zone_aer_add.num == 0 ) 
			sprintf(str_num, "%u", 1 );

		else 
			sprintf(str_num, "%u", board_db->zone_aer_db[ j-1 ].zone_aer_add.num + 1 );
		return;
	}
	default:
		return;

}

}

/* Выставляем валидность данных */
/* Translate: Set validation of data */
void set_valid( s_board_db_t* board_db, uint num_board_db, enum type_db current )
{
	switch ( current ) {
		case geo_db: {
			board_db->geo_db[ num_board_db - 1 ].geo_add.valid.num 						= 1;
			board_db->geo_db[ num_board_db - 1 ].geo_add.valid.name						= 1; 
			board_db->geo_db[ num_board_db - 1 ].geo_add.valid.lat 						= 1;
			board_db->geo_db[ num_board_db - 1 ].geo_add.valid.lon 						= 1;
			board_db->geo_db[ num_board_db - 1 ].geo_add.valid.hAbs 					= 1;
			board_db->geo_db[ num_board_db - 1 ].geo_add.valid.declination 				= 1;
			board_db->geo_db[ num_board_db - 1 ].geo_add.valid.editability				= 1;
			board_db->geo_db[ num_board_db - 1 ].geo_add.valid.deletable				= 1;
			return;
		}
		case aer_db: {
			board_db->aer_db[ num_board_db - 1 ].aer_add.valid.num 						= 1;
			board_db->aer_db[ num_board_db - 1 ].aer_add.valid.name						= 1; 
			board_db->aer_db[ num_board_db - 1 ].aer_add.valid.lat 						= 1;
			board_db->aer_db[ num_board_db - 1 ].aer_add.valid.lon 						= 1;
			board_db->aer_db[ num_board_db - 1 ].aer_add.valid.hAbs 					= 1;
			board_db->aer_db[ num_board_db - 1 ].aer_add.valid.declination 				= 1;
			board_db->aer_db[ num_board_db - 1 ].aer_add.valid.editability				= 1;
			board_db->aer_db[ num_board_db - 1 ].aer_add.valid.deletable				= 1;
			return;
		}
		case mrsh_db: {
			board_db->mrsh_db[ num_board_db - 1 ].mrsh_add.valid.num 					= 1;
			board_db->mrsh_db[ num_board_db - 1 ].mrsh_add.valid.name					= 1; 
			board_db->mrsh_db[ num_board_db - 1 ].mrsh_add.valid.lat 					= 1;
			board_db->mrsh_db[ num_board_db - 1 ].mrsh_add.valid.lon 					= 1;
			board_db->mrsh_db[ num_board_db - 1 ].mrsh_add.valid.hAbs 					= 1;
			board_db->mrsh_db[ num_board_db - 1 ].mrsh_add.valid.declination 			= 1;
			board_db->mrsh_db[ num_board_db - 1 ].mrsh_add.valid.editability 			= 1;
			board_db->mrsh_db[ num_board_db - 1 ].mrsh_add.valid.deletable				= 1;
			return;	
		}
		case vor_db: {
			board_db->vor_db[ num_board_db - 1 ].vor_add.valid.num 						= 1;
			board_db->vor_db[ num_board_db - 1 ].vor_add.valid.name						= 1; 
			board_db->vor_db[ num_board_db - 1 ].vor_add.valid.lat 						= 1;
			board_db->vor_db[ num_board_db - 1 ].vor_add.valid.lon 						= 1;
			board_db->vor_db[ num_board_db - 1 ].vor_add.valid.hAbs 					= 1;
			board_db->vor_db[ num_board_db - 1 ].vor_add.valid.declination 				= 1;
			board_db->vor_db[ num_board_db - 1 ].vor_add.valid.editability				= 1;
			board_db->vor_db[ num_board_db - 1 ].vor_add.valid.deletable				= 1;
			return;
		}
		case zone_zap_db: {
			board_db->zone_zap_db[ num_board_db - 1 ].zone_zap_add.valid.num 			= 1;
			board_db->zone_zap_db[ num_board_db - 1 ].zone_zap_add.valid.name			= 1; 
			board_db->zone_zap_db[ num_board_db - 1 ].zone_zap_add.valid.lat 			= 1;
			board_db->zone_zap_db[ num_board_db - 1 ].zone_zap_add.valid.lon 			= 1;
			board_db->zone_zap_db[ num_board_db - 1 ].zone_zap_add.valid.hAbs 			= 1;
			board_db->zone_zap_db[ num_board_db - 1 ].zone_zap_add.valid.declination 	= 1;
			board_db->zone_zap_db[ num_board_db - 1 ].zone_zap_add.valid.editability	= 1;
			board_db->zone_zap_db[ num_board_db - 1 ].zone_zap_add.valid.deletable		= 1;
			return;
		}
		case zone_plt_db: {
			board_db->zone_plt_db[ num_board_db - 1 ].zone_plt_add.valid.num 			= 1;
			board_db->zone_plt_db[ num_board_db - 1 ].zone_plt_add.valid.name			= 1; 
			board_db->zone_plt_db[ num_board_db - 1 ].zone_plt_add.valid.lat 			= 1;
			board_db->zone_plt_db[ num_board_db - 1 ].zone_plt_add.valid.lon 			= 1;
			board_db->zone_plt_db[ num_board_db - 1 ].zone_plt_add.valid.hAbs 			= 1;
			board_db->zone_plt_db[ num_board_db - 1 ].zone_plt_add.valid.declination 	= 1;
			board_db->zone_plt_db[ num_board_db - 1 ].zone_plt_add.valid.editability	= 1;
			board_db->zone_plt_db[ num_board_db - 1 ].zone_plt_add.valid.deletable		= 1;
			return;
		}
		case zone_aer_db: {
			board_db->zone_aer_db[ num_board_db - 1 ].zone_aer_add.valid.num 			= 1;
			board_db->zone_aer_db[ num_board_db - 1 ].zone_aer_add.valid.name			= 1; 
			board_db->zone_aer_db[ num_board_db - 1 ].zone_aer_add.valid.lat 			= 1;
			board_db->zone_aer_db[ num_board_db - 1 ].zone_aer_add.valid.lon 			= 1;
			board_db->zone_aer_db[ num_board_db - 1 ].zone_aer_add.valid.hAbs 			= 1;
			board_db->zone_aer_db[ num_board_db - 1 ].zone_aer_add.valid.declination 	= 1;
			board_db->zone_aer_db[ num_board_db - 1 ].zone_aer_add.valid.editability	= 1;
			board_db->zone_aer_db[ num_board_db - 1 ].zone_aer_add.valid.deletable		= 1;
			return;
		}
		case rsbn_db: {
			board_db->rsbn_db[ num_board_db - 1 ].rsbn_add.valid.num 					= 1;
			board_db->rsbn_db[ num_board_db - 1 ].rsbn_add.valid.name					= 1; 
			board_db->rsbn_db[ num_board_db - 1 ].rsbn_add.valid.lat 					= 1;
			board_db->rsbn_db[ num_board_db - 1 ].rsbn_add.valid.lon 					= 1;
			board_db->rsbn_db[ num_board_db - 1 ].rsbn_add.valid.hAbs 					= 1;
			board_db->rsbn_db[ num_board_db - 1 ].rsbn_add.valid.declination 			= 1;
			board_db->rsbn_db[ num_board_db - 1 ].rsbn_add.valid.editability			= 1;
			board_db->rsbn_db[ num_board_db - 1 ].rsbn_add.valid.deletable				= 1;
			return;
		}
		default:
			return;
	}
}


void edit_board_db( GtkWidget *widget, gpointer data, int size_board_db, char warning[], enum type_db current )
{
    s_board_db_t *board_db  = ( s_board_db_t * )data;

    GtkWidget *form, *dialog;
    GtkWidget *entry_num, *entry_name, *entry_lat, *entry_lon, *entry_hAbs, *entry_declination;

    form = gtk_vbox_new(FALSE, 0);

    // Создание формы
	// Translate: creating form
    entry_num = gtk_entry_new();
    gtk_box_pack_start( GTK_BOX( form ), gtk_label_new( "Enter number of point to edit:"), FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( form ), entry_num, FALSE, FALSE, 0 );

    dialog = gtk_dialog_new_with_buttons( "Edit point", NULL, GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL );
    gtk_container_add( GTK_CONTAINER( gtk_dialog_get_content_area( GTK_DIALOG( dialog ) ) ), form );
    gtk_widget_show_all( dialog );

    while ( TRUE ) {
        if ( gtk_dialog_run( GTK_DIALOG( dialog ) ) == GTK_RESPONSE_ACCEPT ) {
            int num_to_edit = strtoul( gtk_entry_get_text( GTK_ENTRY( entry_num ) ), NULL, 10 );
			switch ( current ) {
				case geo_db: {
					if ( num_to_edit > 0 && num_to_edit <= size_board_db && board_db->geo_db[ num_to_edit - 1 ].geo_add.valid.editability == 1 ) {
				        // Create a new form to edit the geopoint
				        GtkWidget *edit_form = gtk_vbox_new( FALSE, 0 );

				        entry_name = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Name:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_name, FALSE, FALSE, 0 );

				        entry_lat = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Latitude:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_lat, FALSE, FALSE, 0 );

				        entry_lon = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Longitude:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_lon, FALSE, FALSE, 0 );
		 
				        entry_hAbs = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Absolute Height:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_hAbs, FALSE, FALSE, 0 );

				        entry_declination = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Declination:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_declination, FALSE, FALSE, 0 );

				        GtkWidget *edit_dialog = gtk_dialog_new_with_buttons( "Edit Geopoint", NULL, GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL );
				        gtk_container_add( GTK_CONTAINER( gtk_dialog_get_content_area( GTK_DIALOG( edit_dialog ) ) ), edit_form );
				        gtk_widget_show_all( edit_dialog );

				        while ( TRUE ) {
							//board_db->geo_db[ num_to_edit - 1 ].geo_add.valid.editability == 1
				            if ( gtk_dialog_run( GTK_DIALOG( edit_dialog ) ) == GTK_RESPONSE_ACCEPT ) {

								// Обновление параметров точки
						        // Translate: Update the geopoint with the new values
						        board_db->geo_db[ num_to_edit - 1 ].geo_add.num 		= num_to_edit;
						        strncpy( board_db->geo_db[ num_to_edit - 1 ].geo_add.name, gtk_entry_get_text( GTK_ENTRY( entry_name ) ), POINT_NAME_SIZE - 1 );
						        board_db->geo_db[ num_to_edit - 1 ].geo_add.lat 		= atof( gtk_entry_get_text( GTK_ENTRY( entry_lat ) ) );
						        board_db->geo_db[ num_to_edit - 1 ].geo_add.lon 		= atof( gtk_entry_get_text( GTK_ENTRY( entry_lon ) ) );
						        board_db->geo_db[ num_to_edit - 1 ].geo_add.hAbs 		= atof( gtk_entry_get_text( GTK_ENTRY( entry_hAbs ) ) );
						        board_db->geo_db[ num_to_edit - 1 ].geo_add.declination = atof( gtk_entry_get_text( GTK_ENTRY( entry_declination ) ) );
								/* Фильтрация данных */
								/* Translate: filter data */
								if ( board_db->geo_db[ num_to_edit - 1 ].geo_add.name[0] == '\0' ) 
								{ 
									GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid name" );
									gtk_dialog_run( GTK_DIALOG( error_dialog ) );
									gtk_widget_destroy( error_dialog );
								}
		
								if ( board_db->geo_db[ num_to_edit - 1 ].geo_add.lat == 0 ) 
								{ 
									GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid latitude" );
									gtk_dialog_run( GTK_DIALOG( error_dialog ) );
									gtk_widget_destroy( error_dialog );
								}
								if ( board_db->geo_db[ num_to_edit - 1 ].geo_add.lon == 0 ) 
								{ 
									GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid longitude" );
									gtk_dialog_run( GTK_DIALOG( error_dialog ) );
									gtk_widget_destroy( error_dialog );
								}
				
								if ( board_db->geo_db[ num_to_edit - 1 ].geo_add.name > 0 &&
									board_db->geo_db[ num_to_edit - 1 ].geo_add.lat > 0 &&
									board_db->geo_db[ num_to_edit - 1 ].geo_add.lon > 0 )
								{
									/* Выставляем валидность данных */
									/* Translate: Set validation of data */
									set_valid( board_db, num_to_edit, current );
		
									GtkWidget *confirm_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_OTHER, GTK_BUTTONS_OK, "OK" );
									gtk_dialog_run( GTK_DIALOG( confirm_dialog ) );
									gtk_widget_destroy( confirm_dialog );
			
									break;
								}

				            } else
				                break;
				            
				        }

				        gtk_widget_destroy( edit_dialog );
				    } else {
				        GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid geopoint number" );
				        gtk_dialog_run( GTK_DIALOG( error_dialog ) );
				        gtk_widget_destroy( error_dialog );
				    }
				
				break;
				
				}
				case aer_db: {
					if ( num_to_edit > 0 && num_to_edit <= size_board_db && board_db->aer_db[ num_to_edit - 1 ].aer_add.valid.editability == 1 ) {
				        // Create a new form to edit the geopoint
				        GtkWidget *edit_form = gtk_vbox_new( FALSE, 0 );

				        entry_name = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Name:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_name, FALSE, FALSE, 0 );

				        entry_lat = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Latitude:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_lat, FALSE, FALSE, 0 );

				        entry_lon = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Longitude:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_lon, FALSE, FALSE, 0 );
		 
				        entry_hAbs = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Absolute Height:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_hAbs, FALSE, FALSE, 0 );

				        entry_declination = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Declination:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_declination, FALSE, FALSE, 0 );

				        GtkWidget *edit_dialog = gtk_dialog_new_with_buttons( "Edit Geopoint", NULL, GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL );
				        gtk_container_add( GTK_CONTAINER( gtk_dialog_get_content_area( GTK_DIALOG( edit_dialog ) ) ), edit_form );
				        gtk_widget_show_all( edit_dialog );

				        while ( TRUE ) {
							//board_db->geo_db[ num_to_edit - 1 ].geo_add.valid.editability == 1
				            if ( gtk_dialog_run( GTK_DIALOG( edit_dialog ) ) == GTK_RESPONSE_ACCEPT ) {

								// Обновление параметров точки
						        // Translate: Update the geopoint with the new values
						        board_db->aer_db[ num_to_edit - 1 ].aer_add.num 		= num_to_edit;
						        strncpy( board_db->aer_db[ num_to_edit - 1 ].aer_add.name, gtk_entry_get_text( GTK_ENTRY( entry_name ) ), POINT_NAME_SIZE - 1 );
						        board_db->aer_db[ num_to_edit - 1 ].aer_add.lat 		= atof( gtk_entry_get_text( GTK_ENTRY( entry_lat ) ) );
						        board_db->aer_db[ num_to_edit - 1 ].aer_add.lon 		= atof( gtk_entry_get_text( GTK_ENTRY( entry_lon ) ) );
						        board_db->aer_db[ num_to_edit - 1 ].aer_add.hAbs 		= atof( gtk_entry_get_text( GTK_ENTRY( entry_hAbs ) ) );
						        board_db->aer_db[ num_to_edit - 1 ].aer_add.declination = atof( gtk_entry_get_text( GTK_ENTRY( entry_declination ) ) );
								/* Фильтрация данных */
								/* Translate: filter data */
								if ( board_db->aer_db[ num_to_edit - 1 ].aer_add.name[0] == '\0' ) 
								{ 
									GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid name" );
									gtk_dialog_run( GTK_DIALOG( error_dialog ) );
									gtk_widget_destroy( error_dialog );
								}
		
								if ( board_db->aer_db[ num_to_edit - 1 ].aer_add.lat == 0 ) 
								{ 
									GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid latitude" );
									gtk_dialog_run( GTK_DIALOG( error_dialog ) );
									gtk_widget_destroy( error_dialog );
								}
								if ( board_db->aer_db[ num_to_edit - 1 ].aer_add.lon == 0 ) 
								{ 
									GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid longitude" );
									gtk_dialog_run( GTK_DIALOG( error_dialog ) );
									gtk_widget_destroy( error_dialog );
								}
				
								if ( board_db->aer_db[ num_to_edit - 1 ].aer_add.name > 0 &&
									board_db->aer_db[ num_to_edit - 1 ].aer_add.lat > 0 &&
									board_db->aer_db[ num_to_edit - 1 ].aer_add.lon > 0 )
								{
									/* Выставляем валидность данных */
									/* Translate: Set validation of data */
									set_valid( board_db, num_to_edit, current );
		
									GtkWidget *confirm_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_OTHER, GTK_BUTTONS_OK, "OK" );
									gtk_dialog_run( GTK_DIALOG( confirm_dialog ) );
									gtk_widget_destroy( confirm_dialog );
			
									break;
								}

				            } else
				                break;
				            
				        }

				        gtk_widget_destroy( edit_dialog );
				    } else {
				        GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid geopoint number" );
				        gtk_dialog_run( GTK_DIALOG( error_dialog ) );
				        gtk_widget_destroy( error_dialog );
				    }
				
				break;
				
				}
				case mrsh_db: {
					if ( num_to_edit > 0 && num_to_edit <= size_board_db && board_db->mrsh_db[ num_to_edit - 1 ].mrsh_add.valid.editability == 1 ) {
				        // Create a new form to edit the geopoint
				        GtkWidget *edit_form = gtk_vbox_new( FALSE, 0 );

				        entry_name = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Name:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_name, FALSE, FALSE, 0 );

				        entry_lat = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Latitude:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_lat, FALSE, FALSE, 0 );

				        entry_lon = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Longitude:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_lon, FALSE, FALSE, 0 );
		 
				        entry_hAbs = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Absolute Height:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_hAbs, FALSE, FALSE, 0 );

				        entry_declination = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Declination:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_declination, FALSE, FALSE, 0 );

				        GtkWidget *edit_dialog = gtk_dialog_new_with_buttons( "Edit Geopoint", NULL, GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL );
				        gtk_container_add( GTK_CONTAINER( gtk_dialog_get_content_area( GTK_DIALOG( edit_dialog ) ) ), edit_form );
				        gtk_widget_show_all( edit_dialog );

				        while ( TRUE ) {
							//board_db->geo_db[ num_to_edit - 1 ].geo_add.valid.editability == 1
				            if ( gtk_dialog_run( GTK_DIALOG( edit_dialog ) ) == GTK_RESPONSE_ACCEPT ) {

								// Обновление параметров точки
						        // Translate: Update the geopoint with the new values
						        board_db->mrsh_db[ num_to_edit - 1 ].mrsh_add.num 		= num_to_edit;
						        strncpy( board_db->mrsh_db[ num_to_edit - 1 ].mrsh_add.name, gtk_entry_get_text( GTK_ENTRY( entry_name ) ), POINT_NAME_SIZE - 1 );
						        board_db->mrsh_db[ num_to_edit - 1 ].mrsh_add.lat 		= atof( gtk_entry_get_text( GTK_ENTRY( entry_lat ) ) );
						        board_db->mrsh_db[ num_to_edit - 1 ].mrsh_add.lon 		= atof( gtk_entry_get_text( GTK_ENTRY( entry_lon ) ) );
						        board_db->mrsh_db[ num_to_edit - 1 ].mrsh_add.hAbs 		= atof( gtk_entry_get_text( GTK_ENTRY( entry_hAbs ) ) );
						        board_db->mrsh_db[ num_to_edit - 1 ].mrsh_add.declination = atof( gtk_entry_get_text( GTK_ENTRY( entry_declination ) ) );
								/* Фильтрация данных */
								/* Translate: filter data */
								if ( board_db->mrsh_db[ num_to_edit - 1 ].mrsh_add.name[0] == '\0' ) 
								{ 
									GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid name" );
									gtk_dialog_run( GTK_DIALOG( error_dialog ) );
									gtk_widget_destroy( error_dialog );
								}
		
								if ( board_db->mrsh_db[ num_to_edit - 1 ].mrsh_add.lat == 0 ) 
								{ 
									GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid latitude" );
									gtk_dialog_run( GTK_DIALOG( error_dialog ) );
									gtk_widget_destroy( error_dialog );
								}
								if ( board_db->mrsh_db[ num_to_edit - 1 ].mrsh_add.lon == 0 ) 
								{ 
									GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid longitude" );
									gtk_dialog_run( GTK_DIALOG( error_dialog ) );
									gtk_widget_destroy( error_dialog );
								}
				
								if ( board_db->mrsh_db[ num_to_edit - 1 ].mrsh_add.name > 0 &&
									board_db->mrsh_db[ num_to_edit - 1 ].mrsh_add.lat > 0 &&
									board_db->mrsh_db[ num_to_edit - 1 ].mrsh_add.lon > 0 )
								{
									/* Выставляем валидность данных */
									/* Translate: Set validation of data */
									set_valid( board_db, num_to_edit, current );
		
									GtkWidget *confirm_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_OTHER, GTK_BUTTONS_OK, "OK" );
									gtk_dialog_run( GTK_DIALOG( confirm_dialog ) );
									gtk_widget_destroy( confirm_dialog );
			
									break;
								}

				            } else
				                break;
				            
				        }

				        gtk_widget_destroy( edit_dialog );
				    } else {
				        GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid geopoint number" );
				        gtk_dialog_run( GTK_DIALOG( error_dialog ) );
				        gtk_widget_destroy( error_dialog );
				    }
				
				break;
				
				}
				case rsbn_db: {
					if ( num_to_edit > 0 && num_to_edit <= size_board_db && board_db->rsbn_db[ num_to_edit - 1 ].rsbn_add.valid.editability == 1 ) {
				        // Create a new form to edit the geopoint
				        GtkWidget *edit_form = gtk_vbox_new( FALSE, 0 );

				        entry_name = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Name:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_name, FALSE, FALSE, 0 );

				        entry_lat = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Latitude:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_lat, FALSE, FALSE, 0 );

				        entry_lon = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Longitude:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_lon, FALSE, FALSE, 0 );
		 
				        entry_hAbs = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Absolute Height:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_hAbs, FALSE, FALSE, 0 );

				        entry_declination = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Declination:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_declination, FALSE, FALSE, 0 );

				        GtkWidget *edit_dialog = gtk_dialog_new_with_buttons( "Edit Geopoint", NULL, GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL );
				        gtk_container_add( GTK_CONTAINER( gtk_dialog_get_content_area( GTK_DIALOG( edit_dialog ) ) ), edit_form );
				        gtk_widget_show_all( edit_dialog );

				        while ( TRUE ) {
							//board_db->geo_db[ num_to_edit - 1 ].geo_add.valid.editability == 1
				            if ( gtk_dialog_run( GTK_DIALOG( edit_dialog ) ) == GTK_RESPONSE_ACCEPT ) {

								// Обновление параметров точки
						        // Translate: Update the geopoint with the new values
						        board_db->rsbn_db[ num_to_edit - 1 ].rsbn_add.num 		= num_to_edit;
						        strncpy( board_db->rsbn_db[ num_to_edit - 1 ].rsbn_add.name, gtk_entry_get_text( GTK_ENTRY( entry_name ) ), POINT_NAME_SIZE - 1 );
						        board_db->rsbn_db[ num_to_edit - 1 ].rsbn_add.lat 		= atof( gtk_entry_get_text( GTK_ENTRY( entry_lat ) ) );
						        board_db->rsbn_db[ num_to_edit - 1 ].rsbn_add.lon 		= atof( gtk_entry_get_text( GTK_ENTRY( entry_lon ) ) );
						        board_db->rsbn_db[ num_to_edit - 1 ].rsbn_add.hAbs 		= atof( gtk_entry_get_text( GTK_ENTRY( entry_hAbs ) ) );
						        board_db->rsbn_db[ num_to_edit - 1 ].rsbn_add.declination = atof( gtk_entry_get_text( GTK_ENTRY( entry_declination ) ) );
								/* Фильтрация данных */
								/* Translate: filter data */
								if ( board_db->rsbn_db[ num_to_edit - 1 ].rsbn_add.name[0] == '\0' ) 
								{ 
									GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid name" );
									gtk_dialog_run( GTK_DIALOG( error_dialog ) );
									gtk_widget_destroy( error_dialog );
								}
		
								if ( board_db->rsbn_db[ num_to_edit - 1 ].rsbn_add.lat == 0 ) 
								{ 
									GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid latitude" );
									gtk_dialog_run( GTK_DIALOG( error_dialog ) );
									gtk_widget_destroy( error_dialog );
								}
								if ( board_db->rsbn_db[ num_to_edit - 1 ].rsbn_add.lon == 0 ) 
								{ 
									GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid longitude" );
									gtk_dialog_run( GTK_DIALOG( error_dialog ) );
									gtk_widget_destroy( error_dialog );
								}
				
								if ( board_db->rsbn_db[ num_to_edit - 1 ].rsbn_add.name > 0 &&
									board_db->rsbn_db[ num_to_edit - 1 ].rsbn_add.lat > 0 &&
									board_db->rsbn_db[ num_to_edit - 1 ].rsbn_add.lon > 0 )
								{
									/* Выставляем валидность данных */
									/* Translate: Set validation of data */
									set_valid( board_db, num_to_edit, current );
		
									GtkWidget *confirm_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_OTHER, GTK_BUTTONS_OK, "OK" );
									gtk_dialog_run( GTK_DIALOG( confirm_dialog ) );
									gtk_widget_destroy( confirm_dialog );
			
									break;
								}

				            } else
				                break;
				            
				        }

				        gtk_widget_destroy( edit_dialog );
				    } else {
				        GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid geopoint number" );
				        gtk_dialog_run( GTK_DIALOG( error_dialog ) );
				        gtk_widget_destroy( error_dialog );
				    }
				
				break;
				
				}
				case zone_zap_db: {
					if ( num_to_edit > 0 && num_to_edit <= size_board_db && board_db->zone_zap_db[ num_to_edit - 1 ].zone_zap_add.valid.editability == 1 ) {
				        // Create a new form to edit the geopoint
				        GtkWidget *edit_form = gtk_vbox_new( FALSE, 0 );

				        entry_name = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Name:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_name, FALSE, FALSE, 0 );

				        entry_lat = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Latitude:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_lat, FALSE, FALSE, 0 );

				        entry_lon = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Longitude:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_lon, FALSE, FALSE, 0 );
		 
				        entry_hAbs = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Absolute Height:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_hAbs, FALSE, FALSE, 0 );

				        entry_declination = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Declination:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_declination, FALSE, FALSE, 0 );

				        GtkWidget *edit_dialog = gtk_dialog_new_with_buttons( "Edit Geopoint", NULL, GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL );
				        gtk_container_add( GTK_CONTAINER( gtk_dialog_get_content_area( GTK_DIALOG( edit_dialog ) ) ), edit_form );
				        gtk_widget_show_all( edit_dialog );

				        while ( TRUE ) {
							//board_db->geo_db[ num_to_edit - 1 ].geo_add.valid.editability == 1
				            if ( gtk_dialog_run( GTK_DIALOG( edit_dialog ) ) == GTK_RESPONSE_ACCEPT ) {

								// Обновление параметров точки
						        // Translate: Update the geopoint with the new values
						        board_db->zone_zap_db[ num_to_edit - 1 ].zone_zap_add.num 		= num_to_edit;
						        strncpy( board_db->zone_zap_db[ num_to_edit - 1 ].zone_zap_add.name, gtk_entry_get_text( GTK_ENTRY( entry_name ) ), POINT_NAME_SIZE - 1 );
						        board_db->zone_zap_db[ num_to_edit - 1 ].zone_zap_add.lat 		= atof( gtk_entry_get_text( GTK_ENTRY( entry_lat ) ) );
						        board_db->zone_zap_db[ num_to_edit - 1 ].zone_zap_add.lon 		= atof( gtk_entry_get_text( GTK_ENTRY( entry_lon ) ) );
						        board_db->zone_zap_db[ num_to_edit - 1 ].zone_zap_add.hAbs 		= atof( gtk_entry_get_text( GTK_ENTRY( entry_hAbs ) ) );
						        board_db->zone_zap_db[ num_to_edit - 1 ].zone_zap_add.declination = atof( gtk_entry_get_text( GTK_ENTRY( entry_declination ) ) );
								/* Фильтрация данных */
								/* Translate: filter data */
								if ( board_db->zone_zap_db[ num_to_edit - 1 ].zone_zap_add.name[0] == '\0' ) 
								{ 
									GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid name" );
									gtk_dialog_run( GTK_DIALOG( error_dialog ) );
									gtk_widget_destroy( error_dialog );
								}
		
								if ( board_db->zone_zap_db[ num_to_edit - 1 ].zone_zap_add.lat == 0 ) 
								{ 
									GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid latitude" );
									gtk_dialog_run( GTK_DIALOG( error_dialog ) );
									gtk_widget_destroy( error_dialog );
								}
								if ( board_db->zone_zap_db[ num_to_edit - 1 ].zone_zap_add.lon == 0 ) 
								{ 
									GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid longitude" );
									gtk_dialog_run( GTK_DIALOG( error_dialog ) );
									gtk_widget_destroy( error_dialog );
								}
				
								if ( board_db->zone_zap_db[ num_to_edit - 1 ].zone_zap_add.name > 0 &&
									board_db->zone_zap_db[ num_to_edit - 1 ].zone_zap_add.lat > 0 &&
									board_db->zone_zap_db[ num_to_edit - 1 ].zone_zap_add.lon > 0 )
								{
									/* Выставляем валидность данных */
									/* Translate: Set validation of data */
									set_valid( board_db, num_to_edit, current );
		
									GtkWidget *confirm_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_OTHER, GTK_BUTTONS_OK, "OK" );
									gtk_dialog_run( GTK_DIALOG( confirm_dialog ) );
									gtk_widget_destroy( confirm_dialog );
			
									break;
								}

				            } else
				                break;
				            
				        }

				        gtk_widget_destroy( edit_dialog );
				    } else {
				        GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid geopoint number" );
				        gtk_dialog_run( GTK_DIALOG( error_dialog ) );
				        gtk_widget_destroy( error_dialog );
				    }
				
				break;
				
				}
				case zone_plt_db: {
					if ( num_to_edit > 0 && num_to_edit <= size_board_db && board_db->zone_plt_db[ num_to_edit - 1 ].zone_plt_add.valid.editability == 1 ) {
				        // Create a new form to edit the geopoint
				        GtkWidget *edit_form = gtk_vbox_new( FALSE, 0 );

				        entry_name = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Name:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_name, FALSE, FALSE, 0 );

				        entry_lat = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Latitude:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_lat, FALSE, FALSE, 0 );

				        entry_lon = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Longitude:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_lon, FALSE, FALSE, 0 );
		 
				        entry_hAbs = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Absolute Height:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_hAbs, FALSE, FALSE, 0 );

				        entry_declination = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Declination:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_declination, FALSE, FALSE, 0 );

				        GtkWidget *edit_dialog = gtk_dialog_new_with_buttons( "Edit Geopoint", NULL, GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL );
				        gtk_container_add( GTK_CONTAINER( gtk_dialog_get_content_area( GTK_DIALOG( edit_dialog ) ) ), edit_form );
				        gtk_widget_show_all( edit_dialog );

				        while ( TRUE ) {
							//board_db->geo_db[ num_to_edit - 1 ].geo_add.valid.editability == 1
				            if ( gtk_dialog_run( GTK_DIALOG( edit_dialog ) ) == GTK_RESPONSE_ACCEPT ) {

								// Обновление параметров точки
						        // Translate: Update the geopoint with the new values
						        board_db->zone_plt_db[ num_to_edit - 1 ].zone_plt_add.num 		= num_to_edit;
						        strncpy( board_db->zone_plt_db[ num_to_edit - 1 ].zone_plt_add.name, gtk_entry_get_text( GTK_ENTRY( entry_name ) ), POINT_NAME_SIZE - 1 );
						        board_db->zone_plt_db[ num_to_edit - 1 ].zone_plt_add.lat 		= atof( gtk_entry_get_text( GTK_ENTRY( entry_lat ) ) );
						        board_db->zone_plt_db[ num_to_edit - 1 ].zone_plt_add.lon 		= atof( gtk_entry_get_text( GTK_ENTRY( entry_lon ) ) );
						        board_db->zone_plt_db[ num_to_edit - 1 ].zone_plt_add.hAbs 		= atof( gtk_entry_get_text( GTK_ENTRY( entry_hAbs ) ) );
						        board_db->zone_plt_db[ num_to_edit - 1 ].zone_plt_add.declination = atof( gtk_entry_get_text( GTK_ENTRY( entry_declination ) ) );
								/* Фильтрация данных */
								/* Translate: filter data */
								if ( board_db->zone_plt_db[ num_to_edit - 1 ].zone_plt_add.name[0] == '\0' ) 
								{ 
									GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid name" );
									gtk_dialog_run( GTK_DIALOG( error_dialog ) );
									gtk_widget_destroy( error_dialog );
								}
		
								if ( board_db->zone_plt_db[ num_to_edit - 1 ].zone_plt_add.lat == 0 ) 
								{ 
									GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid latitude" );
									gtk_dialog_run( GTK_DIALOG( error_dialog ) );
									gtk_widget_destroy( error_dialog );
								}
								if ( board_db->zone_plt_db[ num_to_edit - 1 ].zone_plt_add.lon == 0 ) 
								{ 
									GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid longitude" );
									gtk_dialog_run( GTK_DIALOG( error_dialog ) );
									gtk_widget_destroy( error_dialog );
								}
				
								if ( board_db->zone_plt_db[ num_to_edit - 1 ].zone_plt_add.name > 0 &&
									board_db->zone_plt_db[ num_to_edit - 1 ].zone_plt_add.lat > 0 &&
									board_db->zone_plt_db[ num_to_edit - 1 ].zone_plt_add.lon > 0 )
								{
									/* Выставляем валидность данных */
									/* Translate: Set validation of data */
									set_valid( board_db, num_to_edit, current );
		
									GtkWidget *confirm_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_OTHER, GTK_BUTTONS_OK, "OK" );
									gtk_dialog_run( GTK_DIALOG( confirm_dialog ) );
									gtk_widget_destroy( confirm_dialog );
			
									break;
								}

				            } else
				                break;
				            
				        }

				        gtk_widget_destroy( edit_dialog );
				    } else {
				        GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid geopoint number" );
				        gtk_dialog_run( GTK_DIALOG( error_dialog ) );
				        gtk_widget_destroy( error_dialog );
				    }
				
				break;
				
				}
				case zone_aer_db: {
					if ( num_to_edit > 0 && num_to_edit <= size_board_db && board_db->zone_aer_db[ num_to_edit - 1 ].zone_aer_add.valid.editability == 1 ) {
				        // Create a new form to edit the geopoint
				        GtkWidget *edit_form = gtk_vbox_new( FALSE, 0 );

				        entry_name = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Name:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_name, FALSE, FALSE, 0 );

				        entry_lat = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Latitude:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_lat, FALSE, FALSE, 0 );

				        entry_lon = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Longitude:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_lon, FALSE, FALSE, 0 );
		 
				        entry_hAbs = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Absolute Height:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_hAbs, FALSE, FALSE, 0 );

				        entry_declination = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Declination:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_declination, FALSE, FALSE, 0 );

				        GtkWidget *edit_dialog = gtk_dialog_new_with_buttons( "Edit Geopoint", NULL, GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL );
				        gtk_container_add( GTK_CONTAINER( gtk_dialog_get_content_area( GTK_DIALOG( edit_dialog ) ) ), edit_form );
				        gtk_widget_show_all( edit_dialog );

				        while ( TRUE ) {
							//board_db->geo_db[ num_to_edit - 1 ].geo_add.valid.editability == 1
				            if ( gtk_dialog_run( GTK_DIALOG( edit_dialog ) ) == GTK_RESPONSE_ACCEPT ) {

								// Обновление параметров точки
						        // Translate: Update the geopoint with the new values
						        board_db->zone_aer_db[ num_to_edit - 1 ].zone_aer_add.num 		= num_to_edit;
						        strncpy( board_db->zone_aer_db[ num_to_edit - 1 ].zone_aer_add.name, gtk_entry_get_text( GTK_ENTRY( entry_name ) ), POINT_NAME_SIZE - 1 );
						        board_db->zone_aer_db[ num_to_edit - 1 ].zone_aer_add.lat 		= atof( gtk_entry_get_text( GTK_ENTRY( entry_lat ) ) );
						        board_db->zone_aer_db[ num_to_edit - 1 ].zone_aer_add.lon 		= atof( gtk_entry_get_text( GTK_ENTRY( entry_lon ) ) );
						        board_db->zone_aer_db[ num_to_edit - 1 ].zone_aer_add.hAbs 		= atof( gtk_entry_get_text( GTK_ENTRY( entry_hAbs ) ) );
						        board_db->zone_aer_db[ num_to_edit - 1 ].zone_aer_add.declination = atof( gtk_entry_get_text( GTK_ENTRY( entry_declination ) ) );
								/* Фильтрация данных */
								/* Translate: filter data */
								if ( board_db->zone_aer_db[ num_to_edit - 1 ].zone_aer_add.name[0] == '\0' ) 
								{ 
									GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid name" );
									gtk_dialog_run( GTK_DIALOG( error_dialog ) );
									gtk_widget_destroy( error_dialog );
								}
		
								if ( board_db->zone_aer_db[ num_to_edit - 1 ].zone_aer_add.lat == 0 ) 
								{ 
									GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid latitude" );
									gtk_dialog_run( GTK_DIALOG( error_dialog ) );
									gtk_widget_destroy( error_dialog );
								}
								if ( board_db->zone_aer_db[ num_to_edit - 1 ].zone_aer_add.lon == 0 ) 
								{ 
									GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid longitude" );
									gtk_dialog_run( GTK_DIALOG( error_dialog ) );
									gtk_widget_destroy( error_dialog );
								}
				
								if ( board_db->zone_aer_db[ num_to_edit - 1 ].zone_aer_add.name > 0 &&
									board_db->zone_aer_db[ num_to_edit - 1 ].zone_aer_add.lat > 0 &&
									board_db->zone_aer_db[ num_to_edit - 1 ].zone_aer_add.lon > 0 )
								{
									/* Выставляем валидность данных */
									/* Translate: Set validation of data */
									set_valid( board_db, num_to_edit, current );
		
									GtkWidget *confirm_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_OTHER, GTK_BUTTONS_OK, "OK" );
									gtk_dialog_run( GTK_DIALOG( confirm_dialog ) );
									gtk_widget_destroy( confirm_dialog );
			
									break;
								}

				            } else
				                break;
				            
				        }

				        gtk_widget_destroy( edit_dialog );
				    } else {
				        GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid geopoint number" );
				        gtk_dialog_run( GTK_DIALOG( error_dialog ) );
				        gtk_widget_destroy( error_dialog );
				    }
				
				break;
				
				}
				case vor_db: {
					if ( num_to_edit > 0 && num_to_edit <= size_board_db && board_db->vor_db[ num_to_edit - 1 ].vor_add.valid.editability == 1 ) {
				        // Create a new form to edit the geopoint
				        GtkWidget *edit_form = gtk_vbox_new( FALSE, 0 );

				        entry_name = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Name:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_name, FALSE, FALSE, 0 );

				        entry_lat = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Latitude:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_lat, FALSE, FALSE, 0 );

				        entry_lon = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Longitude:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_lon, FALSE, FALSE, 0 );
		 
				        entry_hAbs = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Absolute Height:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_hAbs, FALSE, FALSE, 0 );

				        entry_declination = gtk_entry_new();
				        gtk_box_pack_start( GTK_BOX( edit_form ), gtk_label_new( "Declination:" ), FALSE, FALSE, 0 );
				        gtk_box_pack_start( GTK_BOX( edit_form ), entry_declination, FALSE, FALSE, 0 );

				        GtkWidget *edit_dialog = gtk_dialog_new_with_buttons( "Edit Geopoint", NULL, GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL );
				        gtk_container_add( GTK_CONTAINER( gtk_dialog_get_content_area( GTK_DIALOG( edit_dialog ) ) ), edit_form );
				        gtk_widget_show_all( edit_dialog );

				        while ( TRUE ) {
							//board_db->geo_db[ num_to_edit - 1 ].geo_add.valid.editability == 1
				            if ( gtk_dialog_run( GTK_DIALOG( edit_dialog ) ) == GTK_RESPONSE_ACCEPT ) {

								// Обновление параметров точки
						        // Translate: Update the geopoint with the new values
						        board_db->vor_db[ num_to_edit - 1 ].vor_add.num 		= num_to_edit;
						        strncpy( board_db->vor_db[ num_to_edit - 1 ].vor_add.name, gtk_entry_get_text( GTK_ENTRY( entry_name ) ), POINT_NAME_SIZE - 1 );
						        board_db->vor_db[ num_to_edit - 1 ].vor_add.lat 		= atof( gtk_entry_get_text( GTK_ENTRY( entry_lat ) ) );
						        board_db->vor_db[ num_to_edit - 1 ].vor_add.lon 		= atof( gtk_entry_get_text( GTK_ENTRY( entry_lon ) ) );
						        board_db->vor_db[ num_to_edit - 1 ].vor_add.hAbs 		= atof( gtk_entry_get_text( GTK_ENTRY( entry_hAbs ) ) );
						        board_db->vor_db[ num_to_edit - 1 ].vor_add.declination = atof( gtk_entry_get_text( GTK_ENTRY( entry_declination ) ) );
								/* Фильтрация данных */
								/* Translate: filter data */
								if ( board_db->vor_db[ num_to_edit - 1 ].vor_add.name[0] == '\0' ) 
								{ 
									GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid name" );
									gtk_dialog_run( GTK_DIALOG( error_dialog ) );
									gtk_widget_destroy( error_dialog );
								}
		
								if ( board_db->vor_db[ num_to_edit - 1 ].vor_add.lat == 0 ) 
								{ 
									GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid latitude" );
									gtk_dialog_run( GTK_DIALOG( error_dialog ) );
									gtk_widget_destroy( error_dialog );
								}
								if ( board_db->vor_db[ num_to_edit - 1 ].vor_add.lon == 0 ) 
								{ 
									GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid longitude" );
									gtk_dialog_run( GTK_DIALOG( error_dialog ) );
									gtk_widget_destroy( error_dialog );
								}
				
								if ( board_db->vor_db[ num_to_edit - 1 ].vor_add.name > 0 &&
									board_db->vor_db[ num_to_edit - 1 ].vor_add.lat > 0 &&
									board_db->vor_db[ num_to_edit - 1 ].vor_add.lon > 0 )
								{
									/* Выставляем валидность данных */
									/* Translate: Set validation of data */
									set_valid( board_db, num_to_edit, current );
		
									GtkWidget *confirm_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_OTHER, GTK_BUTTONS_OK, "OK" );
									gtk_dialog_run( GTK_DIALOG( confirm_dialog ) );
									gtk_widget_destroy( confirm_dialog );
			
									break;
								}

				            } else
				                break;
				            
				        }

				        gtk_widget_destroy( edit_dialog );
				    } else {
				        GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid geopoint number" );
				        gtk_dialog_run( GTK_DIALOG( error_dialog ) );
				        gtk_widget_destroy( error_dialog );
				    }
				
				break;
				
				}
			
				default:
					break;
			}

        } else 
            break;

        
    }

	/* Проверяем валидность данных. Коррекция данных */
	/* Translate: Check validate. Correction of data */
	
	check_valid( board_db, current );
	
    gtk_widget_destroy( dialog );
}

void add_board_db( GtkWidget *widget, gpointer data, int size_board_db, char warning[], enum type_db current )
{

    s_board_db_t *board_db  = ( s_board_db_t * )data;
    int num_board_db = 0; // добавляем статическую переменную для индексации
						  // Translate: adding a static variable for indexing
    GtkWidget *form, *dialog, *button;
    GtkWidget *entry_num, *entry_name, *entry_lat, *entry_lon, *entry_hAbs, *entry_declination;
    form = gtk_vbox_new(FALSE, 0);
	
    // Создание формы
    entry_num = gtk_entry_new();
    gtk_box_pack_start( GTK_BOX( form ), gtk_label_new( "Number:" ), FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( form ), entry_num, FALSE, FALSE, 0 );

    entry_name = gtk_entry_new();
    gtk_box_pack_start( GTK_BOX( form ), gtk_label_new( "Name:" ), FALSE, FALSE, 0);
    gtk_box_pack_start( GTK_BOX( form ), entry_name, FALSE, FALSE, 0 );

    entry_lat = gtk_entry_new();
    gtk_box_pack_start( GTK_BOX( form ), gtk_label_new( "Latitude:" ), FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( form ), entry_lat, FALSE, FALSE, 0 );

    entry_lon = gtk_entry_new();
    gtk_box_pack_start( GTK_BOX( form ), gtk_label_new( "Longitude:" ), FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( form ), entry_lon, FALSE, FALSE, 0 );

    entry_hAbs = gtk_entry_new();
    gtk_box_pack_start( GTK_BOX( form ), gtk_label_new( "Absolute Height:" ), FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( form ), entry_hAbs, FALSE, FALSE, 0 );

    entry_declination = gtk_entry_new();
    gtk_box_pack_start( GTK_BOX( form ), gtk_label_new( "Declination:" ), FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( form ), entry_declination, FALSE, FALSE, 0 );

/*	button = gtk_button_new_with_label( " REFRESH " );
    gtk_box_pack_start( GTK_BOX( form ), button, FALSE, FALSE, 0 );
	g_signal_connect( button, "clicked", G_CALLBACK( clear_values ), &board_db );*/
	
	/* Автоматически дописываем номер */
	/* Translate: automatic write num */
	char point_num[20];

	write_number( board_db, point_num, size_board_db, current );
	
	gtk_entry_set_text( GTK_ENTRY( entry_num ), point_num) ;
	
	dialog = gtk_dialog_new_with_buttons( "Add point", NULL, GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL );
    gtk_container_add( GTK_CONTAINER( gtk_dialog_get_content_area( GTK_DIALOG( dialog ) ) ), form );
    gtk_widget_show_all( dialog );
	while ( TRUE ) {

		if ( gtk_dialog_run( GTK_DIALOG( dialog ) ) == GTK_RESPONSE_ACCEPT )
		{
			num_board_db = strtoul( gtk_entry_get_text( GTK_ENTRY( entry_num ) ), NULL, 10 );
		    // Добавление данных в структуру board_db
			// Translate: Adding data to the board_db structure
		    if ( num_board_db <= size_board_db && num_board_db > 0 ) // проверяем, что не превышен максимальный размер массива

		    {
				switch ( current ) {
					case geo_db: {
						board_db->geo_db[ num_board_db - 1 ].geo_add.num 			= num_board_db;
						strncpy( board_db->geo_db[ num_board_db - 1 ].geo_add.name, gtk_entry_get_text( GTK_ENTRY( entry_name ) ), POINT_NAME_SIZE - 1 );
						board_db->geo_db[ num_board_db - 1 ].geo_add.lat 			= atof( gtk_entry_get_text( GTK_ENTRY( entry_lat ) ) );
						board_db->geo_db[ num_board_db - 1 ].geo_add.lon 			= atof( gtk_entry_get_text( GTK_ENTRY( entry_lon ) ) );
						board_db->geo_db[ num_board_db - 1 ].geo_add.hAbs 			= atof( gtk_entry_get_text( GTK_ENTRY( entry_hAbs ) ) );
						board_db->geo_db[ num_board_db - 1 ].geo_add.declination 	= atof( gtk_entry_get_text( GTK_ENTRY( entry_declination ) ) );
						if ( board_db->geo_db[ num_board_db - 1 ].geo_add.name[0] == '\0' ) 
						{ 
						    GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid name" );
						    gtk_dialog_run( GTK_DIALOG( error_dialog ) );
						    gtk_widget_destroy( error_dialog );
							break;
						}
				
						if ( board_db->geo_db[ num_board_db - 1 ].geo_add.lat == 0 ) 
						{ 
						    GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid latitude" );
						    gtk_dialog_run( GTK_DIALOG( error_dialog ) );
						    gtk_widget_destroy( error_dialog );
							break;
						}
						if ( board_db->geo_db[ num_board_db - 1 ].geo_add.lon == 0 ) 
						{ 
						    GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid longitude" );
						    gtk_dialog_run( GTK_DIALOG( error_dialog ) );
						    gtk_widget_destroy( error_dialog );
							break;
						}
						if ( board_db->geo_db[ num_board_db - 1 ].geo_add.valid.editability == 1 ) {
						    GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "This number is busy" );
						    gtk_dialog_run( GTK_DIALOG( error_dialog ) );
						    gtk_widget_destroy( error_dialog );
							break;
						}
						if ( board_db->geo_db[ num_board_db - 1 ].geo_add.name > 0 &&
							board_db->geo_db[ num_board_db - 1 ].geo_add.lat > 0 &&
							board_db->geo_db[ num_board_db - 1 ].geo_add.lon > 0 )
						{
							/* Выставляем валидность данных */
							/* Translate: Set validation of data */
							set_valid( board_db, num_board_db, current );
				
							GtkWidget *confirm_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_OTHER, GTK_BUTTONS_OK, "OK" );
							gtk_dialog_run( GTK_DIALOG( confirm_dialog ) );
							gtk_widget_destroy( confirm_dialog );
							gtk_widget_destroy( dialog );
					
							break;
						}
					
					break;

					}
					case aer_db: {
						board_db->aer_db[ num_board_db - 1 ].aer_add.num 			= num_board_db;
						strncpy( board_db->aer_db[ num_board_db - 1 ].aer_add.name, gtk_entry_get_text( GTK_ENTRY( entry_name ) ), POINT_NAME_SIZE - 1 );
						board_db->aer_db[ num_board_db - 1 ].aer_add.lat 			= atof( gtk_entry_get_text( GTK_ENTRY( entry_lat ) ) );
						board_db->aer_db[ num_board_db - 1 ].aer_add.lon 			= atof( gtk_entry_get_text( GTK_ENTRY( entry_lon ) ) );
						board_db->aer_db[ num_board_db - 1 ].aer_add.hAbs 			= atof( gtk_entry_get_text( GTK_ENTRY( entry_hAbs ) ) );
						board_db->aer_db[ num_board_db - 1 ].aer_add.declination 	= atof( gtk_entry_get_text( GTK_ENTRY( entry_declination ) ) );
						if ( board_db->aer_db[ num_board_db - 1 ].aer_add.name[0] == '\0' ) 
						{ 
						    GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid name" );
						    gtk_dialog_run( GTK_DIALOG( error_dialog ) );
						    gtk_widget_destroy( error_dialog );
							break;
						}
				
						if ( board_db->aer_db[ num_board_db - 1 ].aer_add.lat == 0 ) 
						{ 
						    GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid latitude" );
						    gtk_dialog_run( GTK_DIALOG( error_dialog ) );
						    gtk_widget_destroy( error_dialog );
							break;
						}
						if ( board_db->aer_db[ num_board_db - 1 ].aer_add.lon == 0 ) 
						{ 
						    GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid longitude" );
						    gtk_dialog_run( GTK_DIALOG( error_dialog ) );
						    gtk_widget_destroy( error_dialog );
							break;
						}
						if ( board_db->aer_db[ num_board_db - 1 ].aer_add.valid.editability == 1 ) {
						    GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "This number is busy" );
						    gtk_dialog_run( GTK_DIALOG( error_dialog ) );
						    gtk_widget_destroy( error_dialog );
							break;
						}
						if ( board_db->aer_db[ num_board_db - 1 ].aer_add.name > 0 &&
							board_db->aer_db[ num_board_db - 1 ].aer_add.lat > 0 &&
							board_db->aer_db[ num_board_db - 1 ].aer_add.lon > 0 )
						{
							/* Выставляем валидность данных */
							/* Translate: Set validation of data */
							set_valid( board_db, num_board_db, current );
				
							GtkWidget *confirm_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_OTHER, GTK_BUTTONS_OK, "OK" );
							gtk_dialog_run( GTK_DIALOG( confirm_dialog ) );
							gtk_widget_destroy( confirm_dialog );
							gtk_widget_destroy( dialog );
					
							break;
						}
					
					break;

					}
					case mrsh_db: {
						board_db->mrsh_db[ num_board_db - 1 ].mrsh_add.num 			= num_board_db;
						strncpy( board_db->mrsh_db[ num_board_db - 1 ].mrsh_add.name, gtk_entry_get_text( GTK_ENTRY( entry_name ) ), POINT_NAME_SIZE - 1 );
						board_db->mrsh_db[ num_board_db - 1 ].mrsh_add.lat 			= atof( gtk_entry_get_text( GTK_ENTRY( entry_lat ) ) );
						board_db->mrsh_db[ num_board_db - 1 ].mrsh_add.lon 			= atof( gtk_entry_get_text( GTK_ENTRY( entry_lon ) ) );
						board_db->mrsh_db[ num_board_db - 1 ].mrsh_add.hAbs 			= atof( gtk_entry_get_text( GTK_ENTRY( entry_hAbs ) ) );
						board_db->mrsh_db[ num_board_db - 1 ].mrsh_add.declination 	= atof( gtk_entry_get_text( GTK_ENTRY( entry_declination ) ) );
						if ( board_db->mrsh_db[ num_board_db - 1 ].mrsh_add.name[0] == '\0' ) 
						{ 
						    GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid name" );
						    gtk_dialog_run( GTK_DIALOG( error_dialog ) );
						    gtk_widget_destroy( error_dialog );
							break;
						}
				
						if ( board_db->mrsh_db[ num_board_db - 1 ].mrsh_add.lat == 0 ) 
						{ 
						    GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid latitude" );
						    gtk_dialog_run( GTK_DIALOG( error_dialog ) );
						    gtk_widget_destroy( error_dialog );
							break;
						}
						if ( board_db->mrsh_db[ num_board_db - 1 ].mrsh_add.lon == 0 ) 
						{ 
						    GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid longitude" );
						    gtk_dialog_run( GTK_DIALOG( error_dialog ) );
						    gtk_widget_destroy( error_dialog );
							break;
						}
						if ( board_db->mrsh_db[ num_board_db - 1 ].mrsh_add.valid.editability == 1 ) {
						    GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "This number is busy" );
						    gtk_dialog_run( GTK_DIALOG( error_dialog ) );
						    gtk_widget_destroy( error_dialog );
							break;
						}
						if ( board_db->mrsh_db[ num_board_db - 1 ].mrsh_add.name > 0 &&
							board_db->mrsh_db[ num_board_db - 1 ].mrsh_add.lat > 0 &&
							board_db->mrsh_db[ num_board_db - 1 ].mrsh_add.lon > 0 )
						{
							/* Выставляем валидность данных */
							/* Translate: Set validation of data */
							set_valid( board_db, num_board_db, current );
				
							GtkWidget *confirm_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_OTHER, GTK_BUTTONS_OK, "OK" );
							gtk_dialog_run( GTK_DIALOG( confirm_dialog ) );
							gtk_widget_destroy( confirm_dialog );
							gtk_widget_destroy( dialog );
					
							break;
						}
					
					break;

					}
					case rsbn_db: {
						board_db->rsbn_db[ num_board_db - 1 ].rsbn_add.num 			= num_board_db;
						strncpy( board_db->rsbn_db[ num_board_db - 1 ].rsbn_add.name, gtk_entry_get_text( GTK_ENTRY( entry_name ) ), POINT_NAME_SIZE - 1 );
						board_db->rsbn_db[ num_board_db - 1 ].rsbn_add.lat 			= atof( gtk_entry_get_text( GTK_ENTRY( entry_lat ) ) );
						board_db->rsbn_db[ num_board_db - 1 ].rsbn_add.lon 			= atof( gtk_entry_get_text( GTK_ENTRY( entry_lon ) ) );
						board_db->rsbn_db[ num_board_db - 1 ].rsbn_add.hAbs 			= atof( gtk_entry_get_text( GTK_ENTRY( entry_hAbs ) ) );
						board_db->rsbn_db[ num_board_db - 1 ].rsbn_add.declination 	= atof( gtk_entry_get_text( GTK_ENTRY( entry_declination ) ) );
						if ( board_db->rsbn_db[ num_board_db - 1 ].rsbn_add.name[0] == '\0' ) 
						{ 
						    GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid name" );
						    gtk_dialog_run( GTK_DIALOG( error_dialog ) );
						    gtk_widget_destroy( error_dialog );
							break;
						}
				
						if ( board_db->rsbn_db[ num_board_db - 1 ].rsbn_add.lat == 0 ) 
						{ 
						    GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid latitude" );
						    gtk_dialog_run( GTK_DIALOG( error_dialog ) );
						    gtk_widget_destroy( error_dialog );
							break;
						}
						if ( board_db->rsbn_db[ num_board_db - 1 ].rsbn_add.lon == 0 ) 
						{ 
						    GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid longitude" );
						    gtk_dialog_run( GTK_DIALOG( error_dialog ) );
						    gtk_widget_destroy( error_dialog );
							break;
						}
						if ( board_db->rsbn_db[ num_board_db - 1 ].rsbn_add.valid.editability == 1 ) {
						    GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "This number is busy" );
						    gtk_dialog_run( GTK_DIALOG( error_dialog ) );
						    gtk_widget_destroy( error_dialog );
							break;
						}
						if ( board_db->rsbn_db[ num_board_db - 1 ].rsbn_add.name > 0 &&
							board_db->rsbn_db[ num_board_db - 1 ].rsbn_add.lat > 0 &&
							board_db->rsbn_db[ num_board_db - 1 ].rsbn_add.lon > 0 )
						{
							/* Выставляем валидность данных */
							/* Translate: Set validation of data */
							set_valid( board_db, num_board_db, current );
				
							GtkWidget *confirm_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_OTHER, GTK_BUTTONS_OK, "OK" );
							gtk_dialog_run( GTK_DIALOG( confirm_dialog ) );
							gtk_widget_destroy( confirm_dialog );
							gtk_widget_destroy( dialog );
					
							break;
						}
					
					break;

					}
					case vor_db: {
						board_db->vor_db[ num_board_db - 1 ].vor_add.num 			= num_board_db;
						strncpy( board_db->vor_db[ num_board_db - 1 ].vor_add.name, gtk_entry_get_text( GTK_ENTRY( entry_name ) ), POINT_NAME_SIZE - 1 );
						board_db->vor_db[ num_board_db - 1 ].vor_add.lat 			= atof( gtk_entry_get_text( GTK_ENTRY( entry_lat ) ) );
						board_db->vor_db[ num_board_db - 1 ].vor_add.lon 			= atof( gtk_entry_get_text( GTK_ENTRY( entry_lon ) ) );
						board_db->vor_db[ num_board_db - 1 ].vor_add.hAbs 			= atof( gtk_entry_get_text( GTK_ENTRY( entry_hAbs ) ) );
						board_db->vor_db[ num_board_db - 1 ].vor_add.declination 	= atof( gtk_entry_get_text( GTK_ENTRY( entry_declination ) ) );
						if ( board_db->vor_db[ num_board_db - 1 ].vor_add.name[0] == '\0' ) 
						{ 
						    GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid name" );
						    gtk_dialog_run( GTK_DIALOG( error_dialog ) );
						    gtk_widget_destroy( error_dialog );
							break;
						}
				
						if ( board_db->vor_db[ num_board_db - 1 ].vor_add.lat == 0 ) 
						{ 
						    GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid latitude" );
						    gtk_dialog_run( GTK_DIALOG( error_dialog ) );
						    gtk_widget_destroy( error_dialog );
							break;
						}
						if ( board_db->vor_db[ num_board_db - 1 ].vor_add.lon == 0 ) 
						{ 
						    GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid longitude" );
						    gtk_dialog_run( GTK_DIALOG( error_dialog ) );
						    gtk_widget_destroy( error_dialog );
							break;
						}
						if ( board_db->vor_db[ num_board_db - 1 ].vor_add.valid.editability == 1 ) {
						    GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "This number is busy" );
						    gtk_dialog_run( GTK_DIALOG( error_dialog ) );
						    gtk_widget_destroy( error_dialog );
							break;
						}
						if ( board_db->vor_db[ num_board_db - 1 ].vor_add.name > 0 &&
							board_db->vor_db[ num_board_db - 1 ].vor_add.lat > 0 &&
							board_db->vor_db[ num_board_db - 1 ].vor_add.lon > 0 )
						{
							/* Выставляем валидность данных */
							/* Translate: Set validation of data */
							set_valid( board_db, num_board_db, current );
				
							GtkWidget *confirm_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_OTHER, GTK_BUTTONS_OK, "OK" );
							gtk_dialog_run( GTK_DIALOG( confirm_dialog ) );
							gtk_widget_destroy( confirm_dialog );
							gtk_widget_destroy( dialog );
					
							break;
						}
					
					break;

					}
					case zone_zap_db: {
						board_db->zone_zap_db[ num_board_db - 1 ].zone_zap_add.num 			= num_board_db;
						strncpy( board_db->zone_zap_db[ num_board_db - 1 ].zone_zap_add.name, gtk_entry_get_text( GTK_ENTRY( entry_name ) ), POINT_NAME_SIZE - 1 );
						board_db->zone_zap_db[ num_board_db - 1 ].zone_zap_add.lat 			= atof( gtk_entry_get_text( GTK_ENTRY( entry_lat ) ) );
						board_db->zone_zap_db[ num_board_db - 1 ].zone_zap_add.lon 			= atof( gtk_entry_get_text( GTK_ENTRY( entry_lon ) ) );
						board_db->zone_zap_db[ num_board_db - 1 ].zone_zap_add.hAbs 			= atof( gtk_entry_get_text( GTK_ENTRY( entry_hAbs ) ) );
						board_db->zone_zap_db[ num_board_db - 1 ].zone_zap_add.declination 	= atof( gtk_entry_get_text( GTK_ENTRY( entry_declination ) ) );
						if ( board_db->zone_zap_db[ num_board_db - 1 ].zone_zap_add.name[0] == '\0' ) 
						{ 
						    GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid name" );
						    gtk_dialog_run( GTK_DIALOG( error_dialog ) );
						    gtk_widget_destroy( error_dialog );
							break;
						}
				
						if ( board_db->zone_zap_db[ num_board_db - 1 ].zone_zap_add.lat == 0 ) 
						{ 
						    GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid latitude" );
						    gtk_dialog_run( GTK_DIALOG( error_dialog ) );
						    gtk_widget_destroy( error_dialog );
							break;
						}
						if ( board_db->zone_zap_db[ num_board_db - 1 ].zone_zap_add.lon == 0 ) 
						{ 
						    GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid longitude" );
						    gtk_dialog_run( GTK_DIALOG( error_dialog ) );
						    gtk_widget_destroy( error_dialog );
							break;
						}
						if ( board_db->zone_zap_db[ num_board_db - 1 ].zone_zap_add.valid.editability == 1 ) {
						    GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "This number is busy" );
						    gtk_dialog_run( GTK_DIALOG( error_dialog ) );
						    gtk_widget_destroy( error_dialog );
							break;
						}
						if ( board_db->zone_zap_db[ num_board_db - 1 ].zone_zap_add.name > 0 &&
							board_db->zone_zap_db[ num_board_db - 1 ].zone_zap_add.lat > 0 &&
							board_db->zone_zap_db[ num_board_db - 1 ].zone_zap_add.lon > 0 )
						{
							/* Выставляем валидность данных */
							/* Translate: Set validation of data */
							set_valid( board_db, num_board_db, current );
				
							GtkWidget *confirm_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_OTHER, GTK_BUTTONS_OK, "OK" );
							gtk_dialog_run( GTK_DIALOG( confirm_dialog ) );
							gtk_widget_destroy( confirm_dialog );
							gtk_widget_destroy( dialog );
					
							break;
						}
					
					break;

					}
					case zone_plt_db: {
						board_db->zone_plt_db[ num_board_db - 1 ].zone_plt_add.num 			= num_board_db;
						strncpy( board_db->zone_plt_db[ num_board_db - 1 ].zone_plt_add.name, gtk_entry_get_text( GTK_ENTRY( entry_name ) ), POINT_NAME_SIZE - 1 );
						board_db->zone_plt_db[ num_board_db - 1 ].zone_plt_add.lat 			= atof( gtk_entry_get_text( GTK_ENTRY( entry_lat ) ) );
						board_db->zone_plt_db[ num_board_db - 1 ].zone_plt_add.lon 			= atof( gtk_entry_get_text( GTK_ENTRY( entry_lon ) ) );
						board_db->zone_plt_db[ num_board_db - 1 ].zone_plt_add.hAbs 			= atof( gtk_entry_get_text( GTK_ENTRY( entry_hAbs ) ) );
						board_db->zone_plt_db[ num_board_db - 1 ].zone_plt_add.declination 	= atof( gtk_entry_get_text( GTK_ENTRY( entry_declination ) ) );
						if ( board_db->zone_plt_db[ num_board_db - 1 ].zone_plt_add.name[0] == '\0' ) 
						{ 
						    GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid name" );
						    gtk_dialog_run( GTK_DIALOG( error_dialog ) );
						    gtk_widget_destroy( error_dialog );
							break;
						}
				
						if ( board_db->zone_plt_db[ num_board_db - 1 ].zone_plt_add.lat == 0 ) 
						{ 
						    GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid latitude" );
						    gtk_dialog_run( GTK_DIALOG( error_dialog ) );
						    gtk_widget_destroy( error_dialog );
							break;
						}
						if ( board_db->zone_plt_db[ num_board_db - 1 ].zone_plt_add.lon == 0 ) 
						{ 
						    GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid longitude" );
						    gtk_dialog_run( GTK_DIALOG( error_dialog ) );
						    gtk_widget_destroy( error_dialog );
							break;
						}
						if ( board_db->zone_plt_db[ num_board_db - 1 ].zone_plt_add.valid.editability == 1 ) {
						    GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "This number is busy" );
						    gtk_dialog_run( GTK_DIALOG( error_dialog ) );
						    gtk_widget_destroy( error_dialog );
							break;
						}
						if ( board_db->zone_plt_db[ num_board_db - 1 ].zone_plt_add.name > 0 &&
							board_db->zone_plt_db[ num_board_db - 1 ].zone_plt_add.lat > 0 &&
							board_db->zone_plt_db[ num_board_db - 1 ].zone_plt_add.lon > 0 )
						{
						    GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "This n" );
						    gtk_dialog_run( GTK_DIALOG( error_dialog ) );
						    gtk_widget_destroy( error_dialog );
							
							/* Выставляем валидность данных */
							/* Translate: Set validation of data */
							set_valid( board_db, num_board_db, current );
				
							GtkWidget *confirm_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_OTHER, GTK_BUTTONS_OK, "OK" );
							gtk_dialog_run( GTK_DIALOG( confirm_dialog ) );
							gtk_widget_destroy( confirm_dialog );
							gtk_widget_destroy( dialog );
					
							break;
						}
					
					break;

					}
					case zone_aer_db: {
						board_db->zone_aer_db[ num_board_db - 1 ].zone_aer_add.num 			= num_board_db;
						strncpy( board_db->zone_aer_db[ num_board_db - 1 ].zone_aer_add.name, gtk_entry_get_text( GTK_ENTRY( entry_name ) ), POINT_NAME_SIZE - 1 );
						board_db->zone_aer_db[ num_board_db - 1 ].zone_aer_add.lat 			= atof( gtk_entry_get_text( GTK_ENTRY( entry_lat ) ) );
						board_db->zone_aer_db[ num_board_db - 1 ].zone_aer_add.lon 			= atof( gtk_entry_get_text( GTK_ENTRY( entry_lon ) ) );
						board_db->zone_aer_db[ num_board_db - 1 ].zone_aer_add.hAbs 			= atof( gtk_entry_get_text( GTK_ENTRY( entry_hAbs ) ) );
						board_db->zone_aer_db[ num_board_db - 1 ].zone_aer_add.declination 	= atof( gtk_entry_get_text( GTK_ENTRY( entry_declination ) ) );
						if ( board_db->zone_aer_db[ num_board_db - 1 ].zone_aer_add.name[0] == '\0' ) 
						{ 
						    GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid name" );
						    gtk_dialog_run( GTK_DIALOG( error_dialog ) );
						    gtk_widget_destroy( error_dialog );
							break;
						}
				
						if ( board_db->zone_aer_db[ num_board_db - 1 ].zone_aer_add.lat == 0 ) 
						{ 
						    GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid latitude" );
						    gtk_dialog_run( GTK_DIALOG( error_dialog ) );
						    gtk_widget_destroy( error_dialog );
							break;
						}
						if ( board_db->zone_aer_db[ num_board_db - 1 ].zone_aer_add.lon == 0 ) 
						{ 
						    GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid longitude" );
						    gtk_dialog_run( GTK_DIALOG( error_dialog ) );
						    gtk_widget_destroy( error_dialog );
							break;
						} 
						if ( board_db->zone_aer_db[ num_board_db - 1 ].zone_aer_add.valid.editability == 1 ) {
						    GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "This number is busy" );
						    gtk_dialog_run( GTK_DIALOG( error_dialog ) );
						    gtk_widget_destroy( error_dialog );
							break;
						}
						if ( board_db->zone_aer_db[ num_board_db - 1 ].zone_aer_add.name > 0 &&
							board_db->zone_aer_db[ num_board_db - 1 ].zone_aer_add.lat > 0 &&
							board_db->zone_aer_db[ num_board_db - 1 ].zone_aer_add.lon > 0 )
						{
							/* Выставляем валидность данных */
							/* Translate: Set validation of data */
							set_valid( board_db, num_board_db, current );
				
							GtkWidget *confirm_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_OTHER, GTK_BUTTONS_OK, "OK" );
							gtk_dialog_run( GTK_DIALOG( confirm_dialog ) );
							gtk_widget_destroy( confirm_dialog );
							gtk_widget_destroy( dialog );
					
							break;
						}
					
					break;

					}
					default:
						break;
				}
			}
		    else
		    {
                GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, warning );
                gtk_dialog_run( GTK_DIALOG( error_dialog ) );
                gtk_widget_destroy( error_dialog );
		    }
		} 
		else 
			break;
		
	}

	/* Проверяем валидность данных. Коррекция данных */
	/* Translate: Check validate. Correction of data */
	check_valid( board_db, current );

	gtk_widget_destroy( dialog );
	
}

void delete_board_db( GtkWidget *widget, gpointer data, int size_board_db, char warning[], enum type_db current )
{
    s_board_db_t *board_db  = ( s_board_db_t * )data;

    GtkWidget *form, *dialog;
    GtkWidget *entry_num;

    form = gtk_vbox_new( FALSE, 0 );

    // Create the form
    entry_num = gtk_entry_new();
    gtk_box_pack_start( GTK_BOX( form ), gtk_label_new( "Enter number of geopoint to delete:" ), FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( form ), entry_num, FALSE, FALSE, 0 );

    dialog = gtk_dialog_new_with_buttons( "Delete point", NULL, GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL );
    gtk_container_add( GTK_CONTAINER( gtk_dialog_get_content_area( GTK_DIALOG( dialog ) ) ), form );
    gtk_widget_show_all( dialog );

    while ( TRUE ) {
        if ( gtk_dialog_run( GTK_DIALOG( dialog ) ) == GTK_RESPONSE_ACCEPT ) {
            int num_to_delete = strtoul( gtk_entry_get_text( GTK_ENTRY( entry_num ) ), NULL, 10 );
			switch ( current ) {
				case geo_db: {
				    if ( num_to_delete > 0 && num_to_delete <= size_board_db && board_db->geo_db[ num_to_delete - 1 ].geo_add.valid.deletable == 1 ) {
				        // Set all parameters of the geopoint to zero
				        board_db->geo_db[ num_to_delete - 1 ].geo_add.num 			= 0;
				        strncpy( board_db->geo_db[ num_to_delete - 1 ].geo_add.name, "", POINT_NAME_SIZE - 1 );
				        board_db->geo_db[ num_to_delete - 1 ].geo_add.lat 			= 0.0;
				        board_db->geo_db[ num_to_delete - 1 ].geo_add.lon 			= 0.0;
				        board_db->geo_db[ num_to_delete - 1 ].geo_add.hAbs 			= 0.0;
				        board_db->geo_db[ num_to_delete - 1 ].geo_add.declination 	= 0.0;
				
						GtkWidget *confirm_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_OTHER, GTK_BUTTONS_OK, "OK" );
						gtk_dialog_run( GTK_DIALOG( confirm_dialog ) );
						gtk_widget_destroy( confirm_dialog );
				    } else {
				        GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid number" );
				        gtk_dialog_run( GTK_DIALOG( error_dialog ) );
				        gtk_widget_destroy( error_dialog );
				    }
					break;
				}
				case aer_db: {
				    if ( num_to_delete > 0 && num_to_delete <= size_board_db && board_db->aer_db[ num_to_delete - 1 ].aer_add.valid.deletable == 1 ) {
				        // Set all parameters of the geopoint to zero
				        board_db->aer_db[ num_to_delete - 1 ].aer_add.num 			= 0;
				        strncpy( board_db->aer_db[ num_to_delete - 1 ].aer_add.name, "", POINT_NAME_SIZE - 1 );
				        board_db->aer_db[ num_to_delete - 1 ].aer_add.lat 			= 0.0;
				        board_db->aer_db[ num_to_delete - 1 ].aer_add.lon 			= 0.0;
				        board_db->aer_db[ num_to_delete - 1 ].aer_add.hAbs 			= 0.0;
				        board_db->aer_db[ num_to_delete - 1 ].aer_add.declination 	= 0.0;
				
						GtkWidget *confirm_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_OTHER, GTK_BUTTONS_OK, "OK" );
						gtk_dialog_run( GTK_DIALOG( confirm_dialog ) );
						gtk_widget_destroy( confirm_dialog );
				    } else {
				        GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid number" );
				        gtk_dialog_run( GTK_DIALOG( error_dialog ) );
				        gtk_widget_destroy( error_dialog );
				    }
					break;
				}
				case rsbn_db: {
				    if ( num_to_delete > 0 && num_to_delete <= size_board_db && board_db->rsbn_db[ num_to_delete - 1 ].rsbn_add.valid.deletable == 1 ) {
				        // Set all parameters of the geopoint to zero
				        board_db->rsbn_db[ num_to_delete - 1 ].rsbn_add.num 			= 0;
				        strncpy( board_db->rsbn_db[ num_to_delete - 1 ].rsbn_add.name, "", POINT_NAME_SIZE - 1 );
				        board_db->rsbn_db[ num_to_delete - 1 ].rsbn_add.lat 			= 0.0;
				        board_db->rsbn_db[ num_to_delete - 1 ].rsbn_add.lon 			= 0.0;
				        board_db->rsbn_db[ num_to_delete - 1 ].rsbn_add.hAbs 			= 0.0;
				        board_db->rsbn_db[ num_to_delete - 1 ].rsbn_add.declination 	= 0.0;
				
						GtkWidget *confirm_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_OTHER, GTK_BUTTONS_OK, "OK" );
						gtk_dialog_run( GTK_DIALOG( confirm_dialog ) );
						gtk_widget_destroy( confirm_dialog );
				    } else {
				        GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid number" );
				        gtk_dialog_run( GTK_DIALOG( error_dialog ) );
				        gtk_widget_destroy( error_dialog );
				    }
					break;
				}
				case vor_db: {
				    if ( num_to_delete > 0 && num_to_delete <= size_board_db && board_db->vor_db[ num_to_delete - 1 ].vor_add.valid.deletable == 1 ) {
				        // Set all parameters of the geopoint to zero
				        board_db->vor_db[ num_to_delete - 1 ].vor_add.num 			= 0;
				        strncpy( board_db->vor_db[ num_to_delete - 1 ].vor_add.name, "", POINT_NAME_SIZE - 1 );
				        board_db->vor_db[ num_to_delete - 1 ].vor_add.lat 			= 0.0;
				        board_db->vor_db[ num_to_delete - 1 ].vor_add.lon 			= 0.0;
				        board_db->vor_db[ num_to_delete - 1 ].vor_add.hAbs 			= 0.0;
				        board_db->vor_db[ num_to_delete - 1 ].vor_add.declination 	= 0.0;
				
						GtkWidget *confirm_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_OTHER, GTK_BUTTONS_OK, "OK" );
						gtk_dialog_run( GTK_DIALOG( confirm_dialog ) );
						gtk_widget_destroy( confirm_dialog );
				    } else {
				        GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid number" );
				        gtk_dialog_run( GTK_DIALOG( error_dialog ) );
				        gtk_widget_destroy( error_dialog );
				    }
					break;
				}
				case zone_zap_db: {
				    if ( num_to_delete > 0 && num_to_delete <= size_board_db && board_db->zone_zap_db[ num_to_delete - 1 ].zone_zap_add.valid.deletable == 1 ) {
				        // Set all parameters of the geopoint to zero
				        board_db->zone_zap_db[ num_to_delete - 1 ].zone_zap_add.num 			= 0;
				        strncpy( board_db->zone_zap_db[ num_to_delete - 1 ].zone_zap_add.name, "", POINT_NAME_SIZE - 1 );
				        board_db->zone_zap_db[ num_to_delete - 1 ].zone_zap_add.lat 			= 0.0;
				        board_db->zone_zap_db[ num_to_delete - 1 ].zone_zap_add.lon 			= 0.0;
				        board_db->zone_zap_db[ num_to_delete - 1 ].zone_zap_add.hAbs 			= 0.0;
				        board_db->zone_zap_db[ num_to_delete - 1 ].zone_zap_add.declination 	= 0.0;
				
						GtkWidget *confirm_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_OTHER, GTK_BUTTONS_OK, "OK" );
						gtk_dialog_run( GTK_DIALOG( confirm_dialog ) );
						gtk_widget_destroy( confirm_dialog );
				    } else {
				        GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid number" );
				        gtk_dialog_run( GTK_DIALOG( error_dialog ) );
				        gtk_widget_destroy( error_dialog );
				    }
					break;
				}
				case zone_plt_db: {
				    if ( num_to_delete > 0 && num_to_delete <= size_board_db && board_db->zone_plt_db[ num_to_delete - 1 ].zone_plt_add.valid.deletable == 1 ) {
				        // Set all parameters of the geopoint to zero
				        board_db->zone_plt_db[ num_to_delete - 1 ].zone_plt_add.num 			= 0;
				        strncpy( board_db->zone_plt_db[ num_to_delete - 1 ].zone_plt_add.name, "", POINT_NAME_SIZE - 1 );
				        board_db->zone_plt_db[ num_to_delete - 1 ].zone_plt_add.lat 			= 0.0;
				        board_db->zone_plt_db[ num_to_delete - 1 ].zone_plt_add.lon 			= 0.0;
				        board_db->zone_plt_db[ num_to_delete - 1 ].zone_plt_add.hAbs 			= 0.0;
				        board_db->zone_plt_db[ num_to_delete - 1 ].zone_plt_add.declination 	= 0.0;
				
						GtkWidget *confirm_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_OTHER, GTK_BUTTONS_OK, "OK" );
						gtk_dialog_run( GTK_DIALOG( confirm_dialog ) );
						gtk_widget_destroy( confirm_dialog );
				    } else {
				        GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid number" );
				        gtk_dialog_run( GTK_DIALOG( error_dialog ) );
				        gtk_widget_destroy( error_dialog );
				    }
					break;
				}
				case zone_aer_db: {
				    if ( num_to_delete > 0 && num_to_delete <= size_board_db && board_db->zone_aer_db[ num_to_delete - 1 ].zone_aer_add.valid.deletable == 1 ) {
				        // Set all parameters of the geopoint to zero
				        board_db->zone_aer_db[ num_to_delete - 1 ].zone_aer_add.num 			= 0;
				        strncpy( board_db->zone_aer_db[ num_to_delete - 1 ].zone_aer_add.name, "", POINT_NAME_SIZE - 1 );
				        board_db->zone_aer_db[ num_to_delete - 1 ].zone_aer_add.lat 			= 0.0;
				        board_db->zone_aer_db[ num_to_delete - 1 ].zone_aer_add.lon 			= 0.0;
				        board_db->zone_aer_db[ num_to_delete - 1 ].zone_aer_add.hAbs 			= 0.0;
				        board_db->zone_aer_db[ num_to_delete - 1 ].zone_aer_add.declination 	= 0.0;
				
						GtkWidget *confirm_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_OTHER, GTK_BUTTONS_OK, "OK" );
						gtk_dialog_run( GTK_DIALOG( confirm_dialog ) );
						gtk_widget_destroy( confirm_dialog );
				    } else {
				        GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid number" );
				        gtk_dialog_run( GTK_DIALOG( error_dialog ) );
				        gtk_widget_destroy( error_dialog );
				    }
					break;
				}
				case mrsh_db: {
				    if ( num_to_delete > 0 && num_to_delete <= size_board_db && board_db->mrsh_db[ num_to_delete - 1 ].mrsh_add.valid.deletable == 1 ) {
				        // Set all parameters of the geopoint to zero
				        board_db->mrsh_db[ num_to_delete - 1 ].mrsh_add.num 			= 0;
				        strncpy( board_db->mrsh_db[ num_to_delete - 1 ].mrsh_add.name, "", POINT_NAME_SIZE - 1 );
				        board_db->mrsh_db[ num_to_delete - 1 ].mrsh_add.lat 			= 0.0;
				        board_db->mrsh_db[ num_to_delete - 1 ].mrsh_add.lon 			= 0.0;
				        board_db->mrsh_db[ num_to_delete - 1 ].mrsh_add.hAbs 			= 0.0;
				        board_db->mrsh_db[ num_to_delete - 1 ].mrsh_add.declination 	= 0.0;
				
						GtkWidget *confirm_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_OTHER, GTK_BUTTONS_OK, "OK" );
						gtk_dialog_run( GTK_DIALOG( confirm_dialog ) );
						gtk_widget_destroy( confirm_dialog );
				    } else {
				        GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid number" );
				        gtk_dialog_run( GTK_DIALOG( error_dialog ) );
				        gtk_widget_destroy( error_dialog );
				    }
					break;
				}
				default:
					break;
			}
			
		} else 
            break;
        
    }

    gtk_widget_destroy( dialog );
}

void display_point_form( GtkWidget *widget, gpointer data, int size_board_db, char warning[], enum type_db current )
{
    s_board_db_t *board_db  = ( s_board_db_t * )data;

    GtkWidget *form, *dialog;
    GtkWidget *entry_num;

    form = gtk_vbox_new(FALSE, 0);

    // Create the form
    entry_num = gtk_entry_new();
    gtk_box_pack_start( GTK_BOX( form ), gtk_label_new( "Enter number of point to display:" ), FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( form ), entry_num, FALSE, FALSE, 0 );

    dialog = gtk_dialog_new_with_buttons("Display Point", NULL, GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);
    gtk_container_add( GTK_CONTAINER( gtk_dialog_get_content_area( GTK_DIALOG( dialog ) ) ), form );
    gtk_widget_show_all( dialog );

    while ( TRUE ) {
        if ( gtk_dialog_run( GTK_DIALOG( dialog ) ) == GTK_RESPONSE_ACCEPT) {
            int num_to_display = strtoul( gtk_entry_get_text( GTK_ENTRY( entry_num ) ), NULL, 10 );
            if ( num_to_display > 0 && num_to_display <= size_board_db ) {
                // Создание формы для отображения параметров точки
				switch ( current ) {
					case geo_db: {
				        GtkWidget *display_form = gtk_vbox_new( FALSE, 0 );

					    gchar *num_str = g_strdup_printf( "Number: %u", board_db->geo_db[ num_to_display - 1 ].geo_add.num );
					    gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( num_str ), FALSE, FALSE, 0 );
					    g_free( num_str );
					
				        gchar *name_str = g_strdup_printf( "Name: %s", board_db->geo_db[ num_to_display - 1 ].geo_add.name );
				        gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( name_str ), FALSE, FALSE, 0 );
				        g_free( name_str );

				        gchar *lat_str = g_strdup_printf( "Latitude: %f", board_db->geo_db[ num_to_display - 1 ].geo_add.lat );
				        gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( lat_str ), FALSE, FALSE, 0 );
				        g_free( lat_str );

				        gchar *lon_str = g_strdup_printf( "Longitude: %f", board_db->geo_db[ num_to_display - 1 ].geo_add.lon );
				        gtk_box_pack_start(GTK_BOX( display_form ), gtk_label_new( lon_str ), FALSE, FALSE, 0 );
				        g_free( lon_str );

				        gchar *hAbs_str = g_strdup_printf( "Absolute Height: %f", board_db->geo_db[ num_to_display - 1 ].geo_add.hAbs );
				        gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( hAbs_str ), FALSE, FALSE, 0 );
				        g_free( hAbs_str );

				        gchar *declination_str = g_strdup_printf( "Declination: %f", board_db->geo_db[ num_to_display - 1 ].geo_add.declination );
				        gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( declination_str ), FALSE, FALSE, 0 );
				        g_free( declination_str );

				        GtkWidget *display_dialog = gtk_dialog_new_with_buttons( "Display Geopoint", NULL, GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, NULL );
				        gtk_container_add( GTK_CONTAINER( gtk_dialog_get_content_area( GTK_DIALOG( display_dialog ) ) ), display_form );
				        gtk_widget_show_all( display_dialog );

				        gtk_dialog_run( GTK_DIALOG( display_dialog ) );
				        gtk_widget_destroy( display_dialog );

						break;
					}
					case aer_db: {
				        GtkWidget *display_form = gtk_vbox_new( FALSE, 0 );

					    gchar *num_str = g_strdup_printf( "Number: %u", board_db->aer_db[ num_to_display - 1 ].aer_add.num );
					    gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( num_str ), FALSE, FALSE, 0 );
					    g_free( num_str );
						
				        gchar *name_str = g_strdup_printf( "Name: %s", board_db->aer_db[ num_to_display - 1 ].aer_add.name );
				        gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( name_str ), FALSE, FALSE, 0 );
				        g_free( name_str );

				        gchar *lat_str = g_strdup_printf( "Latitude: %f", board_db->aer_db[ num_to_display - 1 ].aer_add.lat );
				        gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( lat_str ), FALSE, FALSE, 0 );
				        g_free( lat_str );

				        gchar *lon_str = g_strdup_printf( "Longitude: %f", board_db->aer_db[ num_to_display - 1 ].aer_add.lon );
				        gtk_box_pack_start(GTK_BOX( display_form ), gtk_label_new( lon_str ), FALSE, FALSE, 0 );
				        g_free( lon_str );

				        gchar *hAbs_str = g_strdup_printf( "Absolute Height: %f", board_db->aer_db[ num_to_display - 1 ].aer_add.hAbs );
				        gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( hAbs_str ), FALSE, FALSE, 0 );
				        g_free( hAbs_str );

				        gchar *declination_str = g_strdup_printf( "Declination: %f", board_db->aer_db[ num_to_display - 1 ].aer_add.declination );
				        gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( declination_str ), FALSE, FALSE, 0 );
				        g_free( declination_str );

				        GtkWidget *display_dialog = gtk_dialog_new_with_buttons( "Display Geopoint", NULL, GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, NULL );
				        gtk_container_add( GTK_CONTAINER( gtk_dialog_get_content_area( GTK_DIALOG( display_dialog ) ) ), display_form );
				        gtk_widget_show_all( display_dialog );

				        gtk_dialog_run( GTK_DIALOG( display_dialog ) );
				        gtk_widget_destroy( display_dialog );

						break;
					}
					case rsbn_db: {
				        GtkWidget *display_form = gtk_vbox_new( FALSE, 0 );

					    gchar *num_str = g_strdup_printf( "Number: %u", board_db->rsbn_db[ num_to_display - 1 ].rsbn_add.num );
					    gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( num_str ), FALSE, FALSE, 0 );
					    g_free( num_str );
						
				        gchar *name_str = g_strdup_printf( "Name: %s", board_db->rsbn_db[ num_to_display - 1 ].rsbn_add.name );
				        gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( name_str ), FALSE, FALSE, 0 );
				        g_free( name_str );

				        gchar *lat_str = g_strdup_printf( "Latitude: %f", board_db->rsbn_db[ num_to_display - 1 ].rsbn_add.lat );
				        gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( lat_str ), FALSE, FALSE, 0 );
				        g_free( lat_str );

				        gchar *lon_str = g_strdup_printf( "Longitude: %f", board_db->rsbn_db[ num_to_display - 1 ].rsbn_add.lon );
				        gtk_box_pack_start(GTK_BOX( display_form ), gtk_label_new( lon_str ), FALSE, FALSE, 0 );
				        g_free( lon_str );

				        gchar *hAbs_str = g_strdup_printf( "Absolute Height: %f", board_db->rsbn_db[ num_to_display - 1 ].rsbn_add.hAbs );
				        gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( hAbs_str ), FALSE, FALSE, 0 );
				        g_free( hAbs_str );

				        gchar *declination_str = g_strdup_printf( "Declination: %f", board_db->rsbn_db[ num_to_display - 1 ].rsbn_add.declination );
				        gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( declination_str ), FALSE, FALSE, 0 );
				        g_free( declination_str );

				        GtkWidget *display_dialog = gtk_dialog_new_with_buttons( "Display Geopoint", NULL, GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, NULL );
				        gtk_container_add( GTK_CONTAINER( gtk_dialog_get_content_area( GTK_DIALOG( display_dialog ) ) ), display_form );
				        gtk_widget_show_all( display_dialog );

				        gtk_dialog_run( GTK_DIALOG( display_dialog ) );
				        gtk_widget_destroy( display_dialog );

						break;
					}
					case vor_db: {
				        GtkWidget *display_form = gtk_vbox_new( FALSE, 0 );

					    gchar *num_str = g_strdup_printf( "Number: %u", board_db->vor_db[ num_to_display - 1 ].vor_add.num );
					    gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( num_str ), FALSE, FALSE, 0 );
					    g_free( num_str );
						
				        gchar *name_str = g_strdup_printf( "Name: %s", board_db->vor_db[ num_to_display - 1 ].vor_add.name );
				        gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( name_str ), FALSE, FALSE, 0 );
				        g_free( name_str );

				        gchar *lat_str = g_strdup_printf( "Latitude: %f", board_db->vor_db[ num_to_display - 1 ].vor_add.lat );
				        gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( lat_str ), FALSE, FALSE, 0 );
				        g_free( lat_str );

				        gchar *lon_str = g_strdup_printf( "Longitude: %f", board_db->vor_db[ num_to_display - 1 ].vor_add.lon );
				        gtk_box_pack_start(GTK_BOX( display_form ), gtk_label_new( lon_str ), FALSE, FALSE, 0 );
				        g_free( lon_str );

				        gchar *hAbs_str = g_strdup_printf( "Absolute Height: %f", board_db->vor_db[ num_to_display - 1 ].vor_add.hAbs );
				        gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( hAbs_str ), FALSE, FALSE, 0 );
				        g_free( hAbs_str );

				        gchar *declination_str = g_strdup_printf( "Declination: %f", board_db->vor_db[ num_to_display - 1 ].vor_add.declination );
				        gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( declination_str ), FALSE, FALSE, 0 );
				        g_free( declination_str );

				        GtkWidget *display_dialog = gtk_dialog_new_with_buttons( "Display Geopoint", NULL, GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, NULL );
				        gtk_container_add( GTK_CONTAINER( gtk_dialog_get_content_area( GTK_DIALOG( display_dialog ) ) ), display_form );
				        gtk_widget_show_all( display_dialog );

				        gtk_dialog_run( GTK_DIALOG( display_dialog ) );
				        gtk_widget_destroy( display_dialog );

						break;
					}
					case mrsh_db: {
				        GtkWidget *display_form = gtk_vbox_new( FALSE, 0 );

					    gchar *num_str = g_strdup_printf( "Number: %u", board_db->mrsh_db[ num_to_display - 1 ].mrsh_add.num );
					    gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( num_str ), FALSE, FALSE, 0 );
					    g_free( num_str );
						
				        gchar *name_str = g_strdup_printf( "Name: %s", board_db->mrsh_db[ num_to_display - 1 ].mrsh_add.name );
				        gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( name_str ), FALSE, FALSE, 0 );
				        g_free( name_str );

				        gchar *lat_str = g_strdup_printf( "Latitude: %f", board_db->mrsh_db[ num_to_display - 1 ].mrsh_add.lat );
				        gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( lat_str ), FALSE, FALSE, 0 );
				        g_free( lat_str );

				        gchar *lon_str = g_strdup_printf( "Longitude: %f", board_db->mrsh_db[ num_to_display - 1 ].mrsh_add.lon );
				        gtk_box_pack_start(GTK_BOX( display_form ), gtk_label_new( lon_str ), FALSE, FALSE, 0 );
				        g_free( lon_str );

				        gchar *hAbs_str = g_strdup_printf( "Absolute Height: %f", board_db->mrsh_db[ num_to_display - 1 ].mrsh_add.hAbs );
				        gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( hAbs_str ), FALSE, FALSE, 0 );
				        g_free( hAbs_str );

				        gchar *declination_str = g_strdup_printf( "Declination: %f", board_db->mrsh_db[ num_to_display - 1 ].mrsh_add.declination );
				        gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( declination_str ), FALSE, FALSE, 0 );
				        g_free( declination_str );

				        GtkWidget *display_dialog = gtk_dialog_new_with_buttons( "Display Geopoint", NULL, GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, NULL );
				        gtk_container_add( GTK_CONTAINER( gtk_dialog_get_content_area( GTK_DIALOG( display_dialog ) ) ), display_form );
				        gtk_widget_show_all( display_dialog );

				        gtk_dialog_run( GTK_DIALOG( display_dialog ) );
				        gtk_widget_destroy( display_dialog );

						break;
					}
					case zone_zap_db: {
				        GtkWidget *display_form = gtk_vbox_new( FALSE, 0 );

					    gchar *num_str = g_strdup_printf( "Number: %u", board_db->zone_zap_db[ num_to_display - 1 ].zone_zap_add.num );
					    gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( num_str ), FALSE, FALSE, 0 );
					    g_free( num_str );
						
				        gchar *name_str = g_strdup_printf( "Name: %s", board_db->zone_zap_db[ num_to_display - 1 ].zone_zap_add.name );
				        gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( name_str ), FALSE, FALSE, 0 );
				        g_free( name_str );

				        gchar *lat_str = g_strdup_printf( "Latitude: %f", board_db->zone_zap_db[ num_to_display - 1 ].zone_zap_add.lat );
				        gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( lat_str ), FALSE, FALSE, 0 );
				        g_free( lat_str );

				        gchar *lon_str = g_strdup_printf( "Longitude: %f", board_db->zone_zap_db[ num_to_display - 1 ].zone_zap_add.lon );
				        gtk_box_pack_start(GTK_BOX( display_form ), gtk_label_new( lon_str ), FALSE, FALSE, 0 );
				        g_free( lon_str );

				        gchar *hAbs_str = g_strdup_printf( "Absolute Height: %f", board_db->zone_zap_db[ num_to_display - 1 ].zone_zap_add.hAbs );
				        gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( hAbs_str ), FALSE, FALSE, 0 );
				        g_free( hAbs_str );

				        gchar *declination_str = g_strdup_printf( "Declination: %f", board_db->zone_zap_db[ num_to_display - 1 ].zone_zap_add.declination );
				        gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( declination_str ), FALSE, FALSE, 0 );
				        g_free( declination_str );

				        GtkWidget *display_dialog = gtk_dialog_new_with_buttons( "Display Geopoint", NULL, GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, NULL );
				        gtk_container_add( GTK_CONTAINER( gtk_dialog_get_content_area( GTK_DIALOG( display_dialog ) ) ), display_form );
				        gtk_widget_show_all( display_dialog );

				        gtk_dialog_run( GTK_DIALOG( display_dialog ) );
				        gtk_widget_destroy( display_dialog );

						break;
					}
					case zone_plt_db: {
				        GtkWidget *display_form = gtk_vbox_new( FALSE, 0 );

					    gchar *num_str = g_strdup_printf( "Number: %u", board_db->zone_plt_db[ num_to_display - 1 ].zone_plt_add.num );
					    gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( num_str ), FALSE, FALSE, 0 );
					    g_free( num_str );
						
				        gchar *name_str = g_strdup_printf( "Name: %s", board_db->zone_plt_db[ num_to_display - 1 ].zone_plt_add.name );
				        gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( name_str ), FALSE, FALSE, 0 );
				        g_free( name_str );

				        gchar *lat_str = g_strdup_printf( "Latitude: %f", board_db->zone_plt_db[ num_to_display - 1 ].zone_plt_add.lat );
				        gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( lat_str ), FALSE, FALSE, 0 );
				        g_free( lat_str );

				        gchar *lon_str = g_strdup_printf( "Longitude: %f", board_db->zone_plt_db[ num_to_display - 1 ].zone_plt_add.lon );
				        gtk_box_pack_start(GTK_BOX( display_form ), gtk_label_new( lon_str ), FALSE, FALSE, 0 );
				        g_free( lon_str );

				        gchar *hAbs_str = g_strdup_printf( "Absolute Height: %f", board_db->zone_plt_db[ num_to_display - 1 ].zone_plt_add.hAbs );
				        gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( hAbs_str ), FALSE, FALSE, 0 );
				        g_free( hAbs_str );

				        gchar *declination_str = g_strdup_printf( "Declination: %f", board_db->zone_plt_db[ num_to_display - 1 ].zone_plt_add.declination );
				        gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( declination_str ), FALSE, FALSE, 0 );
				        g_free( declination_str );

				        GtkWidget *display_dialog = gtk_dialog_new_with_buttons( "Display Geopoint", NULL, GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, NULL );
				        gtk_container_add( GTK_CONTAINER( gtk_dialog_get_content_area( GTK_DIALOG( display_dialog ) ) ), display_form );
				        gtk_widget_show_all( display_dialog );

				        gtk_dialog_run( GTK_DIALOG( display_dialog ) );
				        gtk_widget_destroy( display_dialog );

						break;
					}
					case zone_aer_db: {
				        GtkWidget *display_form = gtk_vbox_new( FALSE, 0 );

					    gchar *num_str = g_strdup_printf( "Number: %u", board_db->zone_aer_db[ num_to_display - 1 ].zone_aer_add.num );
					    gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( num_str ), FALSE, FALSE, 0 );
					    g_free( num_str );
						
				        gchar *name_str = g_strdup_printf( "Name: %s", board_db->zone_aer_db[ num_to_display - 1 ].zone_aer_add.name );
				        gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( name_str ), FALSE, FALSE, 0 );
				        g_free( name_str );

				        gchar *lat_str = g_strdup_printf( "Latitude: %f", board_db->zone_aer_db[ num_to_display - 1 ].zone_aer_add.lat );
				        gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( lat_str ), FALSE, FALSE, 0 );
				        g_free( lat_str );

				        gchar *lon_str = g_strdup_printf( "Longitude: %f", board_db->zone_aer_db[ num_to_display - 1 ].zone_aer_add.lon );
				        gtk_box_pack_start(GTK_BOX( display_form ), gtk_label_new( lon_str ), FALSE, FALSE, 0 );
				        g_free( lon_str );

				        gchar *hAbs_str = g_strdup_printf( "Absolute Height: %f", board_db->zone_aer_db[ num_to_display - 1 ].zone_aer_add.hAbs );
				        gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( hAbs_str ), FALSE, FALSE, 0 );
				        g_free( hAbs_str );

				        gchar *declination_str = g_strdup_printf( "Declination: %f", board_db->zone_aer_db[ num_to_display - 1 ].zone_aer_add.declination );
				        gtk_box_pack_start( GTK_BOX( display_form ), gtk_label_new( declination_str ), FALSE, FALSE, 0 );
				        g_free( declination_str );

				        GtkWidget *display_dialog = gtk_dialog_new_with_buttons( "Display Geopoint", NULL, GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, NULL );
				        gtk_container_add( GTK_CONTAINER( gtk_dialog_get_content_area( GTK_DIALOG( display_dialog ) ) ), display_form );
				        gtk_widget_show_all( display_dialog );

				        gtk_dialog_run( GTK_DIALOG( display_dialog ) );
				        gtk_widget_destroy( display_dialog );

						break;
					}
					default:
						break;
				}
            } else {
                gtk_widget_destroy( dialog );
                GtkWidget *error_dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid number" );
                gtk_dialog_run( GTK_DIALOG( error_dialog ) );
                gtk_widget_destroy( error_dialog );
            }
        } else 
            break;
        
    }

    gtk_widget_destroy( dialog );
}

/* ADD tables */
void 
geo_add( GtkWidget *widget, gpointer data ){
	int table_size = MAX_GEOPOINT_COUNT;
	char warning [256];
	enum type_db db = geo_db;
	snprintf(warning, sizeof(warning), "Invalid number. Max %d!", table_size);
	add_board_db(widget, data, table_size, warning, db );
}
void 
aer_add( GtkWidget *widget, gpointer data ){
	int table_size = MAX_AERDRM_COUNT;
	char warning [256];
	enum type_db db = aer_db;
	snprintf(warning, sizeof(warning), "Invalid number. Max %d!", table_size);
	add_board_db(widget, data, table_size, warning, db );
}
void 
route_add( GtkWidget *widget, gpointer data ){
	int table_size = MAX_ROUTES_COUNT;
	char warning [256];
	enum type_db db = mrsh_db;
	snprintf(warning, sizeof(warning), "Invalid number. Max %d!", table_size);
	add_board_db(widget, data, table_size, warning, db );
}
void 
rsbn_add( GtkWidget *widget, gpointer data ){
	int table_size = MAX_RM_RSBN_COUNT;
	char warning [256];
	enum type_db db = rsbn_db;
	snprintf(warning, sizeof(warning), "Invalid number. Max %d!", table_size);
	add_board_db(widget, data, table_size, warning, db );
}
void 
vor_add( GtkWidget *widget, gpointer data ){
	int table_size = MAX_RM_VOR_COUNT;
	char warning [256];
	enum type_db db = vor_db;
	snprintf(warning, sizeof(warning), "Invalid number. Max %d!", table_size);
	add_board_db(widget, data, table_size, warning, db );
}
void 
zone_zap_add( GtkWidget *widget, gpointer data ){
	int table_size = MAX_ZONE_COUNT;
	char warning [256];
	enum type_db db = zone_zap_db;
	snprintf(warning, sizeof(warning), "Invalid number. Max %d!", table_size);
	add_board_db(widget, data, table_size, warning, db );
}
void 
zone_plt_add( GtkWidget *widget, gpointer data ){
	int table_size = MAX_ZONE_COUNT;
	char warning [256];
	enum type_db db = zone_plt_db;
	snprintf(warning, sizeof(warning), "Invalid number. Max %d!", table_size);
	add_board_db(widget, data, table_size, warning, db );
}
void 
zone_aer_add( GtkWidget *widget, gpointer data ){
	int table_size = MAX_ZONE_COUNT;
	char warning [256];
	enum type_db db = zone_aer_db;
	snprintf(warning, sizeof(warning), "Invalid number. Max %d!", table_size);
	add_board_db(widget, data, table_size, warning, db );
}

/* EDIT tables */
void 
geo_edit( GtkWidget *widget, gpointer data ){
	int table_size = MAX_GEOPOINT_COUNT;
	char warning [256];
	enum type_db db = geo_db;
	snprintf(warning, sizeof(warning), "Invalid number. Max %d!", table_size);
	edit_board_db(widget, data, table_size, warning, db );
}
void 
aer_edit( GtkWidget *widget, gpointer data ){
	int table_size = MAX_AERDRM_COUNT;
	char warning [256];
	enum type_db db = aer_db;
	snprintf(warning, sizeof(warning), "Invalid number. Max %d!", table_size);
	edit_board_db(widget, data, table_size, warning, db );
}
void 
route_edit( GtkWidget *widget, gpointer data ){
	int table_size = MAX_ROUTES_COUNT;
	char warning [256];
	enum type_db db = mrsh_db;
	snprintf(warning, sizeof(warning), "Invalid number. Max %d!", table_size);
	edit_board_db(widget, data, table_size, warning, db );
}
void 
rsbn_edit( GtkWidget *widget, gpointer data ){
	int table_size = MAX_RM_RSBN_COUNT;
	char warning [256];
	enum type_db db = rsbn_db;
	snprintf(warning, sizeof(warning), "Invalid number. Max %d!", table_size);
	edit_board_db(widget, data, table_size, warning, db );
}
void 
vor_edit( GtkWidget *widget, gpointer data ){
	int table_size = MAX_RM_VOR_COUNT;
	char warning [256];
	enum type_db db = vor_db;
	snprintf(warning, sizeof(warning), "Invalid number. Max %d!", table_size);
	edit_board_db(widget, data, table_size, warning, db );
}
void 
zone_zap_edit( GtkWidget *widget, gpointer data ){
	int table_size = MAX_ZONE_COUNT;
	char warning [256];
	enum type_db db = zone_zap_db;
	snprintf(warning, sizeof(warning), "Invalid number. Max %d!", table_size);
	edit_board_db(widget, data, table_size, warning, db );
}
void 
zone_plt_edit( GtkWidget *widget, gpointer data ){
	int table_size = MAX_ZONE_COUNT;
	char warning [256];
	enum type_db db = zone_plt_db;
	snprintf(warning, sizeof(warning), "Invalid number. Max %d!", table_size);
	edit_board_db(widget, data, table_size, warning, db );
}
void 
zone_aer_edit( GtkWidget *widget, gpointer data ){
	int table_size = MAX_ZONE_COUNT;
	char warning [256];
	enum type_db db = zone_aer_db;
	snprintf(warning, sizeof(warning), "Invalid number. Max %d!", table_size);
	edit_board_db(widget, data, table_size, warning, db );
}

/* DELETE tables */
void 
geo_delete( GtkWidget *widget, gpointer data ){
	int table_size = MAX_GEOPOINT_COUNT;
	char warning [256];
	enum type_db db = geo_db;
	snprintf(warning, sizeof(warning), "Invalid number. Max %d!", table_size);
	delete_board_db(widget, data, table_size, warning, db );
}
void 
aer_delete( GtkWidget *widget, gpointer data ){
	int table_size = MAX_AERDRM_COUNT;
	char warning [256];
	enum type_db db = aer_db;
	snprintf(warning, sizeof(warning), "Invalid number. Max %d!", table_size);
	delete_board_db(widget, data, table_size, warning, db );
}
void 
route_delete( GtkWidget *widget, gpointer data ){
	int table_size = MAX_ROUTES_COUNT;
	char warning [256];
	enum type_db db = mrsh_db;
	snprintf(warning, sizeof(warning), "Invalid number. Max %d!", table_size);
	delete_board_db(widget, data, table_size, warning, db );
}
void 
rsbn_delete( GtkWidget *widget, gpointer data ){
	int table_size = MAX_RM_RSBN_COUNT;
	char warning [256];
	enum type_db db = rsbn_db;
	snprintf(warning, sizeof(warning), "Invalid number. Max %d!", table_size);
	delete_board_db(widget, data, table_size, warning, db );
}
void 
vor_delete( GtkWidget *widget, gpointer data ){
	int table_size = MAX_RM_VOR_COUNT;
	char warning [256];
	enum type_db db = vor_db;
	snprintf(warning, sizeof(warning), "Invalid number. Max %d!", table_size);
	delete_board_db(widget, data, table_size, warning, db );
}
void 
zone_zap_delete( GtkWidget *widget, gpointer data ){
	int table_size = MAX_ZONE_COUNT;
	char warning [256];
	enum type_db db = zone_zap_db;
	snprintf(warning, sizeof(warning), "Invalid number. Max %d!", table_size);
	delete_board_db(widget, data, table_size, warning, db );
}
void 
zone_plt_delete( GtkWidget *widget, gpointer data ){
	int table_size = MAX_ZONE_COUNT;
	char warning [256];
	enum type_db db = zone_plt_db;
	snprintf(warning, sizeof(warning), "Invalid number. Max %d!", table_size);
	delete_board_db(widget, data, table_size, warning, db );
}
void 
zone_aer_delete( GtkWidget *widget, gpointer data ){
	int table_size = MAX_ZONE_COUNT;
	char warning [256];
	enum type_db db = zone_aer_db;
	snprintf(warning, sizeof(warning), "Invalid number. Max %d!", table_size);
	delete_board_db(widget, data, table_size, warning, db );
}

/* DISPLAY tables */
void 
geo_show( GtkWidget *widget, gpointer data ){
	int table_size = MAX_GEOPOINT_COUNT;
	char warning [256];
	enum type_db db = geo_db;
	snprintf(warning, sizeof(warning), "Invalid number. Max %d!", table_size);
	display_point_form(widget, data, table_size, warning, db );
}
void 
aer_show( GtkWidget *widget, gpointer data ){
	int table_size = MAX_AERDRM_COUNT;
	char warning [256];
	enum type_db db = aer_db;
	snprintf(warning, sizeof(warning), "Invalid number. Max %d!", table_size);
	display_point_form(widget, data, table_size, warning, db );
}
void 
route_show( GtkWidget *widget, gpointer data ){
	int table_size = MAX_ROUTES_COUNT;
	char warning [256];
	enum type_db db = mrsh_db;
	snprintf(warning, sizeof(warning), "Invalid number. Max %d!", table_size);
	display_point_form(widget, data, table_size, warning, db );
}
void 
rsbn_show( GtkWidget *widget, gpointer data ){
	int table_size = MAX_RM_RSBN_COUNT;
	char warning [256];
	enum type_db db = rsbn_db;
	snprintf(warning, sizeof(warning), "Invalid number. Max %d!", table_size);
	display_point_form(widget, data, table_size, warning, db );
}
void 
vor_show( GtkWidget *widget, gpointer data ){
	int table_size = MAX_RM_VOR_COUNT;
	char warning [256];
	enum type_db db = vor_db;
	snprintf(warning, sizeof(warning), "Invalid number. Max %d!", table_size);
	display_point_form(widget, data, table_size, warning, db );
}
void 
zone_zap_show( GtkWidget *widget, gpointer data ){
	int table_size = MAX_ZONE_COUNT;
	char warning [256];
	enum type_db db = zone_zap_db;
	snprintf(warning, sizeof(warning), "Invalid number. Max %d!", table_size);
	display_point_form(widget, data, table_size, warning, db );
}
void 
zone_plt_show( GtkWidget *widget, gpointer data ){
	int table_size = MAX_ZONE_COUNT;
	char warning [256];
	enum type_db db = zone_plt_db;
	snprintf(warning, sizeof(warning), "Invalid number. Max %d!", table_size);
	display_point_form(widget, data, table_size, warning, db );
}
void 
zone_aer_show( GtkWidget *widget, gpointer data ){
	int table_size = MAX_ZONE_COUNT;
	char warning [256];
	enum type_db db = zone_aer_db;
	snprintf(warning, sizeof(warning), "Invalid number. Max %d!", table_size);
	display_point_form(widget, data, table_size, warning, db );
}

/* READ tables */
void 
geo_read( GtkWidget *widget, gpointer data ){
	int table_size = MAX_GEOPOINT_COUNT;
	char warning [256];
	enum type_db db = geo_db;
	snprintf(warning, sizeof(warning), "\n~~Geopoint DB~~");
	read_board_db(widget, data, table_size, warning, db );
}
void 
aer_read( GtkWidget *widget, gpointer data ){
	int table_size = MAX_AERDRM_COUNT;
	char warning [256];
	enum type_db db = aer_db;
	snprintf(warning, sizeof(warning), "\n~~Aerpoint DB~~");
	read_board_db(widget, data, table_size, warning, db );
}
void 
route_read( GtkWidget *widget, gpointer data ){
	int table_size = MAX_ROUTES_COUNT;
	char warning [256];
	enum type_db db = mrsh_db;
	snprintf(warning, sizeof(warning), "\n~~Mrsh DB~~");
	read_board_db(widget, data, table_size, warning, db );
}
void 
rsbn_read( GtkWidget *widget, gpointer data ){
	int table_size = MAX_RM_RSBN_COUNT;
	char warning [256];
	enum type_db db = rsbn_db;
	snprintf(warning, sizeof(warning), "\n~~RSBN DB~~");
	read_board_db(widget, data, table_size, warning, db );
}
void 
vor_read( GtkWidget *widget, gpointer data ){
	int table_size = MAX_RM_VOR_COUNT;
	char warning [256];
	enum type_db db = vor_db;
	snprintf(warning, sizeof(warning), "\n~~VOR DB~~");
	read_board_db(widget, data, table_size, warning, db );
}
void 
zone_zap_read( GtkWidget *widget, gpointer data ){
	int table_size = MAX_ZONE_COUNT;
	char warning [256];
	enum type_db db = zone_zap_db;
	snprintf(warning, sizeof(warning), "\n~~Zone Zap DB~~");
	read_board_db(widget, data, table_size, warning, db );
}
void 
zone_plt_read( GtkWidget *widget, gpointer data ){
	int table_size = MAX_ZONE_COUNT;
	char warning [256];
	enum type_db db = zone_plt_db;
	snprintf(warning, sizeof(warning), "\n~~Zone Pilot DB~~");
	read_board_db(widget, data, table_size, warning, db );
}
void 
zone_aer_read( GtkWidget *widget, gpointer data ){
	int table_size = MAX_ZONE_COUNT;
	char warning [256];
	enum type_db db = zone_aer_db;
	snprintf(warning, sizeof(warning), "\n~~Zone AER DB~~");
	read_board_db(widget, data, table_size, warning, db );
}


void
geo_database( GtkWidget *widget, gpointer data )
{
	
	GtkWidget *window, *vbox, *button;
	s_board_db_t *board_db  = ( s_board_db_t * )data;

    window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
    gtk_window_set_title( GTK_WINDOW( window ), "GEO DB" );

    vbox = gtk_vbox_new( FALSE, 0 );
    gtk_container_add( GTK_CONTAINER( window ), vbox );

    button = gtk_button_new_with_label( " ADD " );
    gtk_box_pack_start( GTK_BOX( vbox ), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( geo_add ), board_db );

    button = gtk_button_new_with_label( " EDIT " );
    gtk_box_pack_start( GTK_BOX(vbox), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( geo_edit ), board_db );

    button = gtk_button_new_with_label( " DELETE " );
    gtk_box_pack_start( GTK_BOX( vbox ), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( geo_delete ), board_db );

    button = gtk_button_new_with_label( " SAVE " );
    gtk_box_pack_start( GTK_BOX( vbox ), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( write_board_db ), board_db );

    button = gtk_button_new_with_label( " SHOW " );
    gtk_box_pack_start( GTK_BOX(vbox), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( geo_show ), board_db );

	button = gtk_button_new_with_label( " READ " );
    gtk_box_pack_start( GTK_BOX(vbox), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( geo_read ), board_db );
	
    gtk_widget_show_all( window );
	
}

void
aer_database( GtkWidget *widget, gpointer data )
{
		
	GtkWidget *window, *vbox, *button;
	s_board_db_t *board_db  = ( s_board_db_t * )data;

    window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
    gtk_window_set_title( GTK_WINDOW( window ), "AER DB" );

    vbox = gtk_vbox_new( FALSE, 0 );
    gtk_container_add( GTK_CONTAINER( window ), vbox );

    button = gtk_button_new_with_label( " ADD " );
    gtk_box_pack_start( GTK_BOX( vbox ), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( aer_add ), board_db );

    button = gtk_button_new_with_label( " EDIT " );
    gtk_box_pack_start( GTK_BOX(vbox), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( aer_edit ), board_db );

    button = gtk_button_new_with_label( " DELETE " );
    gtk_box_pack_start( GTK_BOX( vbox ), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( aer_delete ), board_db );

    button = gtk_button_new_with_label( " SAVE " );
    gtk_box_pack_start( GTK_BOX( vbox ), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( write_board_db ), board_db );

    button = gtk_button_new_with_label( " SHOW " );
    gtk_box_pack_start( GTK_BOX(vbox), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( aer_show ), board_db );

	button = gtk_button_new_with_label( " READ " );
    gtk_box_pack_start( GTK_BOX(vbox), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( aer_read ), board_db );
	
    gtk_widget_show_all( window );
	
}

void
route_database( GtkWidget *widget, gpointer data )
{
	GtkWidget *window, *vbox, *button;
	s_board_db_t *board_db  = ( s_board_db_t * )data;

    window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
    gtk_window_set_title( GTK_WINDOW( window ), "ROUTE DB" );

    vbox = gtk_vbox_new( FALSE, 0 );
    gtk_container_add( GTK_CONTAINER( window ), vbox );

    button = gtk_button_new_with_label( " ADD " );
    gtk_box_pack_start( GTK_BOX( vbox ), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( route_add ), board_db );

    button = gtk_button_new_with_label( " EDIT " );
    gtk_box_pack_start( GTK_BOX(vbox), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( route_edit ), board_db );

    button = gtk_button_new_with_label( " DELETE " );
    gtk_box_pack_start( GTK_BOX( vbox ), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( route_delete ), board_db );

    button = gtk_button_new_with_label( " SAVE" );
    gtk_box_pack_start( GTK_BOX( vbox ), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( write_board_db ), board_db );

    button = gtk_button_new_with_label( " SHOW " );
    gtk_box_pack_start( GTK_BOX(vbox), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( route_show ), board_db );

	button = gtk_button_new_with_label( " READ " );
    gtk_box_pack_start( GTK_BOX(vbox), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( route_read ), board_db );
	
    gtk_widget_show_all( window );
	
}

void
rsbn_database( GtkWidget *widget, gpointer data )
{
	GtkWidget *window, *vbox, *button;
	s_board_db_t *board_db  = ( s_board_db_t * )data;

    window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
    gtk_window_set_title( GTK_WINDOW( window ), "RSBN DB" );

    vbox = gtk_vbox_new( FALSE, 0 );
    gtk_container_add( GTK_CONTAINER( window ), vbox );

    button = gtk_button_new_with_label( " ADD " );
    gtk_box_pack_start( GTK_BOX( vbox ), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( rsbn_add ), board_db );

    button = gtk_button_new_with_label( " EDIT " );
    gtk_box_pack_start( GTK_BOX(vbox), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( rsbn_edit ), board_db );

    button = gtk_button_new_with_label( " DELETE " );
    gtk_box_pack_start( GTK_BOX( vbox ), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( rsbn_delete ), board_db );

    button = gtk_button_new_with_label( " SAVE " );
    gtk_box_pack_start( GTK_BOX( vbox ), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( write_board_db ), board_db );

    button = gtk_button_new_with_label( " SHOW " );
    gtk_box_pack_start( GTK_BOX(vbox), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( rsbn_show ), board_db );

	button = gtk_button_new_with_label( " READ " );
    gtk_box_pack_start( GTK_BOX(vbox), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( rsbn_read ), board_db );
	
    gtk_widget_show_all( window );
	
}

void
vor_database( GtkWidget *widget, gpointer data )
{
	GtkWidget *window, *vbox, *button;
	s_board_db_t *board_db  = ( s_board_db_t * )data;

    window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
    gtk_window_set_title( GTK_WINDOW( window ), "VOR DB" );

    vbox = gtk_vbox_new( FALSE, 0 );
    gtk_container_add( GTK_CONTAINER( window ), vbox );

    button = gtk_button_new_with_label( " ADD " );
    gtk_box_pack_start( GTK_BOX( vbox ), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( vor_add ), board_db );

    button = gtk_button_new_with_label( " EDIT " );
    gtk_box_pack_start( GTK_BOX(vbox), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( vor_edit ), board_db );

    button = gtk_button_new_with_label( " DELETE " );
    gtk_box_pack_start( GTK_BOX( vbox ), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( vor_delete ), board_db );

    button = gtk_button_new_with_label( " SAVE " );
    gtk_box_pack_start( GTK_BOX( vbox ), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( write_board_db ), board_db );

    button = gtk_button_new_with_label( " SHOW " );
    gtk_box_pack_start( GTK_BOX(vbox), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( vor_show ), board_db );

	button = gtk_button_new_with_label( " READ " );
    gtk_box_pack_start( GTK_BOX(vbox), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( vor_read ), board_db );
	
    gtk_widget_show_all( window );
	
}

void
zone_zap_database( GtkWidget *widget, gpointer data )
{
	GtkWidget *window, *vbox, *button;
	s_board_db_t *board_db  = ( s_board_db_t * )data;

    window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
    gtk_window_set_title( GTK_WINDOW( window ), "ZONE ZAP DB" );

    vbox = gtk_vbox_new( FALSE, 0 );
    gtk_container_add( GTK_CONTAINER( window ), vbox );

    button = gtk_button_new_with_label( " ADD " );
    gtk_box_pack_start( GTK_BOX( vbox ), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( zone_zap_add ), board_db );

    button = gtk_button_new_with_label( " EDIT " );
    gtk_box_pack_start( GTK_BOX(vbox), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( zone_zap_edit ), board_db );

    button = gtk_button_new_with_label( " DELETE " );
    gtk_box_pack_start( GTK_BOX( vbox ), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( zone_zap_delete ), board_db );

    button = gtk_button_new_with_label( " SAVE " );
    gtk_box_pack_start( GTK_BOX( vbox ), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( write_board_db ), board_db );

    button = gtk_button_new_with_label( " SHOW " );
    gtk_box_pack_start( GTK_BOX(vbox), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( zone_zap_show ), board_db );

	button = gtk_button_new_with_label( " READ " );
    gtk_box_pack_start( GTK_BOX(vbox), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( zone_zap_read ), board_db );
	
    gtk_widget_show_all( window );
	
}

void
zone_plt_database( GtkWidget *widget, gpointer data )
{
	GtkWidget *window, *vbox, *button;
	s_board_db_t *board_db  = ( s_board_db_t * )data;
	
    window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
    gtk_window_set_title( GTK_WINDOW( window ), "ZONE PIL DB" );

    vbox = gtk_vbox_new( FALSE, 0 );
    gtk_container_add( GTK_CONTAINER( window ), vbox );

    button = gtk_button_new_with_label( " ADD " );
    gtk_box_pack_start( GTK_BOX( vbox ), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( zone_plt_add ), board_db );

    button = gtk_button_new_with_label( " EDIT " );
    gtk_box_pack_start( GTK_BOX(vbox), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( zone_plt_edit ), board_db );

    button = gtk_button_new_with_label( " DELETE " );
    gtk_box_pack_start( GTK_BOX( vbox ), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( zone_plt_delete ), board_db );

    button = gtk_button_new_with_label( " SAVE " );
    gtk_box_pack_start( GTK_BOX( vbox ), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( write_board_db ), board_db );

    button = gtk_button_new_with_label( " SHOW " );
    gtk_box_pack_start( GTK_BOX(vbox), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( zone_plt_show ), board_db );

	button = gtk_button_new_with_label( " READ " );
    gtk_box_pack_start( GTK_BOX(vbox), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( zone_plt_read ), board_db );
	
    gtk_widget_show_all( window );
	
}

void
zone_aer_database( GtkWidget *widget, gpointer data )
{
	GtkWidget *window, *vbox, *button;
	s_board_db_t *board_db  = ( s_board_db_t * )data;

    window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
    gtk_window_set_title( GTK_WINDOW( window ), "ZONE AER DB" );

    vbox = gtk_vbox_new( FALSE, 0 );
    gtk_container_add( GTK_CONTAINER( window ), vbox );

    button = gtk_button_new_with_label( " ADD " );
    gtk_box_pack_start( GTK_BOX( vbox ), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( zone_aer_add ), board_db);
	
    button = gtk_button_new_with_label( " EDIT " );
    gtk_box_pack_start( GTK_BOX(vbox), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( zone_aer_edit ), board_db );

    button = gtk_button_new_with_label( " DELETE " );
    gtk_box_pack_start( GTK_BOX( vbox ), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( zone_aer_delete ), board_db );

    button = gtk_button_new_with_label( " SAVE " );
    gtk_box_pack_start( GTK_BOX( vbox ), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( write_board_db ), board_db );

    button = gtk_button_new_with_label( " SHOW " );
    gtk_box_pack_start( GTK_BOX(vbox), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( zone_aer_show ), board_db );

	button = gtk_button_new_with_label( " READ " );
    gtk_box_pack_start( GTK_BOX(vbox), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( zone_aer_read ), board_db );
	
    gtk_widget_show_all( window );
	
}

int
main ( int argc, char *argv[] )
{
 	GtkWidget *window, *vbox, *button;
/*	s_board_db_t board_db	[ MAX_GEOPOINT_COUNT + MAX_AERDRM_COUNT +
			                  MAX_ROUTES_COUNT + MAX_RM_RSBN_COUNT  +
			                  MAX_RM_VOR_COUNT + MAX_ZONE_COUNT * 3 ];*/
	s_board_db_t board_db;
	
#ifdef ENABLE_NLS

	bindtextdomain ( GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR );
	bind_textdomain_codeset ( GETTEXT_PACKAGE, "UTF-8" );
	textdomain ( GETTEXT_PACKAGE );
#endif
	gtk_init ( &argc, &argv );

    window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
    gtk_window_set_title( GTK_WINDOW( window ), "Board DB" );

    vbox = gtk_vbox_new( FALSE, 0 );
    gtk_container_add( GTK_CONTAINER( window ), vbox );

	button = gtk_button_new_with_label( "GEO DB" );
    gtk_box_pack_start( GTK_BOX( vbox ), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( geo_database ), &board_db.geo_db );

	button = gtk_button_new_with_label( "AER DB" );
    gtk_box_pack_start( GTK_BOX( vbox ), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( aer_database ), &board_db.aer_db );
	
	button = gtk_button_new_with_label( "ROUTE DB" );
    gtk_box_pack_start( GTK_BOX( vbox ), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( route_database ), &board_db.mrsh_db );

	button = gtk_button_new_with_label( "RSBN DB" );
    gtk_box_pack_start( GTK_BOX( vbox ), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( rsbn_database ), &board_db.rsbn_db );

	button = gtk_button_new_with_label( "VOR DB" );
    gtk_box_pack_start( GTK_BOX( vbox ), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( vor_database ), &board_db.vor_db );

	button = gtk_button_new_with_label( "ZONE ZAP DB" );
    gtk_box_pack_start( GTK_BOX( vbox ), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( zone_zap_database ), &board_db.zone_zap_db );

	button = gtk_button_new_with_label( "ZONE PLT DB" );
    gtk_box_pack_start( GTK_BOX( vbox ), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( zone_plt_database ), &board_db.zone_plt_db );

	button = gtk_button_new_with_label( "ZONE AER DB" );
    gtk_box_pack_start( GTK_BOX( vbox ), button, FALSE, FALSE, 0 );
    g_signal_connect( button, "clicked", G_CALLBACK( zone_aer_database ), &board_db.zone_aer_db );
	
	
    gtk_widget_show_all( window );


    gtk_main ();

    g_free ( priv );

	return 0;
}