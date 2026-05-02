#include <iostream>
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <string>

using namespace std;

SQLHENV hEnv;
SQLHDBC hDbc;

// ================= CONEXION =================

void conectar() {
    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
    SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);

    SQLCHAR dsn[] = "universidad";

    SQLRETURN ret = SQLConnect(hDbc, dsn, SQL_NTS, NULL, 0, NULL, 0);

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
        cout << "Conexion exitosa.\n";
    else
        cout << "Error de conexion.\n";
}

void cerrarConexion() {
    SQLDisconnect(hDbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
    SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
}

bool ejecutarSQL(string query, string mensajeOk, string mensajeError) {
    SQLHSTMT hStmt;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    SQLRETURN ret = SQLExecDirect(hStmt, (SQLCHAR*)query.c_str(), SQL_NTS);

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        cout << mensajeOk << endl;
        return true;
    } else {
        cout << mensajeError << endl;
        return false;
    }
}

// ================= ALUMNOS =================

void listarAlumnos() {
    SQLHSTMT hStmt;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    SQLCHAR query[] =
        "SELECT a.id, a.nombres, a.apellidos, a.carnet, "
        "ISNULL(c.nombre, 'Sin cursos') AS curso, "
        "ISNULL(s.nombre, 'Sin seccion') AS seccion "
        "FROM universidad.dbo.alumnos a "
        "LEFT JOIN universidad.dbo.alumnos_cursos ac ON a.id = ac.alumno_id "
        "LEFT JOIN universidad.dbo.cursos c ON ac.curso_id = c.id "
        "LEFT JOIN universidad.dbo.alumnos_seccion ase ON a.id = ase.alumno_id "
        "LEFT JOIN universidad.dbo.secciones s ON ase.seccion_id = s.id";

    SQLRETURN ret = SQLExecDirect(hStmt, query, SQL_NTS);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        cout << "Error al listar alumnos.\n";
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return;
    }

    SQLINTEGER id;
    SQLCHAR nombres[100], apellidos[100], carnet[50], curso[100], seccion[100];

    cout << "\n=== LISTADO DE ALUMNOS ===\n";

    while (true) {
        ret = SQLFetch(hStmt);
        if (ret == SQL_NO_DATA) break;

        if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
            SQLGetData(hStmt, 1, SQL_C_LONG, &id, 0, NULL);
            SQLGetData(hStmt, 2, SQL_C_CHAR, nombres, sizeof(nombres), NULL);
            SQLGetData(hStmt, 3, SQL_C_CHAR, apellidos, sizeof(apellidos), NULL);
            SQLGetData(hStmt, 4, SQL_C_CHAR, carnet, sizeof(carnet), NULL);
            SQLGetData(hStmt, 5, SQL_C_CHAR, curso, sizeof(curso), NULL);
            SQLGetData(hStmt, 6, SQL_C_CHAR, seccion, sizeof(seccion), NULL);

            cout << "ID: " << id
                 << " | " << nombres << " " << apellidos
                 << " | Carnet: " << carnet
                 << " | Curso: " << curso
                 << " | Seccion: " << seccion << endl;
        }
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

void insertarAlumno() {
    string nombres, apellidos, carnet;

    cout << "\n=== REGISTRAR ALUMNO ===\n";
    cout << "Nombres: ";
    cin >> nombres;
    cout << "Apellidos: ";
    cin >> apellidos;
    cout << "Carnet: ";
    cin >> carnet;

    string query =
        "INSERT INTO universidad.dbo.alumnos (nombres, apellidos, carnet) VALUES ('" +
        nombres + "','" + apellidos + "','" + carnet + "')";

    ejecutarSQL(query, "Alumno guardado correctamente.", "Error al guardar alumno.");
}

void modificarAlumno() {
    int id;
    string nombres, apellidos, carnet;

    cout << "\n=== MODIFICAR ALUMNO ===\n";
    cout << "ID del alumno: ";
    cin >> id;
    cout << "Nuevo nombre: ";
    cin >> nombres;
    cout << "Nuevo apellido: ";
    cin >> apellidos;
    cout << "Nuevo carnet: ";
    cin >> carnet;

    string query =
        "UPDATE universidad.dbo.alumnos SET nombres='" + nombres +
        "', apellidos='" + apellidos +
        "', carnet='" + carnet +
        "' WHERE id=" + to_string(id);

    ejecutarSQL(query, "Alumno modificado correctamente.", "Error al modificar alumno.");
}

void eliminarAlumno() {
    int id;

    cout << "\n=== ELIMINAR ALUMNO ===\n";
    cout << "ID del alumno: ";
    cin >> id;

    string query = "DELETE FROM universidad.dbo.alumnos WHERE id=" + to_string(id);

    ejecutarSQL(query,
                "Alumno eliminado correctamente.",
                "Error al eliminar alumno. Puede tener cursos o seccion asignada.");
}

void menuAlumnos() {
    int opcion;

    do {
        cout << "\n===== MODULO ALUMNOS =====\n";
        cout << "1. Listar\n";
        cout << "2. Grabar\n";
        cout << "3. Modificar\n";
        cout << "4. Eliminar\n";
        cout << "5. Regresar\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch (opcion) {
            case 1: listarAlumnos(); break;
            case 2: insertarAlumno(); break;
            case 3: modificarAlumno(); break;
            case 4: eliminarAlumno(); break;
            case 5: break;
            default: cout << "Opcion invalida.\n";
        }

    } while (opcion != 5);
}

// ================= CURSOS =================

void listarCursos() {
    SQLHSTMT hStmt;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    SQLCHAR query[] = "SELECT id, nombre, codigo FROM universidad.dbo.cursos";
    SQLRETURN ret = SQLExecDirect(hStmt, query, SQL_NTS);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        cout << "Error al listar cursos.\n";
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return;
    }

    SQLINTEGER id;
    SQLCHAR nombre[100], codigo[50];

    cout << "\n=== LISTADO DE CURSOS ===\n";

    while (true) {
        ret = SQLFetch(hStmt);
        if (ret == SQL_NO_DATA) break;

        SQLGetData(hStmt, 1, SQL_C_LONG, &id, 0, NULL);
        SQLGetData(hStmt, 2, SQL_C_CHAR, nombre, sizeof(nombre), NULL);
        SQLGetData(hStmt, 3, SQL_C_CHAR, codigo, sizeof(codigo), NULL);

        cout << "ID: " << id << " | Curso: " << nombre << " | Codigo: " << codigo << endl;
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

void insertarCurso() {
    string nombre, codigo;

    cout << "\n=== REGISTRAR CURSO ===\n";
    cout << "Nombre: ";
    cin >> nombre;
    cout << "Codigo: ";
    cin >> codigo;

    string query =
        "INSERT INTO universidad.dbo.cursos (nombre, codigo) VALUES ('" +
        nombre + "','" + codigo + "')";

    ejecutarSQL(query, "Curso guardado correctamente.", "Error al guardar curso.");
}

void modificarCurso() {
    int id;
    string nombre, codigo;

    cout << "\n=== MODIFICAR CURSO ===\n";
    cout << "ID del curso: ";
    cin >> id;
    cout << "Nuevo nombre: ";
    cin >> nombre;
    cout << "Nuevo codigo: ";
    cin >> codigo;

    string query =
        "UPDATE universidad.dbo.cursos SET nombre='" + nombre +
        "', codigo='" + codigo +
        "' WHERE id=" + to_string(id);

    ejecutarSQL(query, "Curso modificado correctamente.", "Error al modificar curso.");
}

void eliminarCurso() {
    int id;

    cout << "\n=== ELIMINAR CURSO ===\n";
    cout << "ID del curso: ";
    cin >> id;

    string query = "DELETE FROM universidad.dbo.cursos WHERE id=" + to_string(id);

    ejecutarSQL(query,
                "Curso eliminado correctamente.",
                "Error al eliminar curso. Puede estar asignado a alumnos.");
}

void menuCursos() {
    int opcion;

    do {
        cout << "\n===== MODULO CURSOS =====\n";
        cout << "1. Listar\n";
        cout << "2. Grabar\n";
        cout << "3. Modificar\n";
        cout << "4. Eliminar\n";
        cout << "5. Regresar\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch (opcion) {
            case 1: listarCursos(); break;
            case 2: insertarCurso(); break;
            case 3: modificarCurso(); break;
            case 4: eliminarCurso(); break;
            case 5: break;
            default: cout << "Opcion invalida.\n";
        }

    } while (opcion != 5);
}

// ================= SECCIONES =================

void listarSecciones() {
    SQLHSTMT hStmt;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    SQLCHAR query[] = "SELECT id, nombre, jornada FROM universidad.dbo.secciones";
    SQLRETURN ret = SQLExecDirect(hStmt, query, SQL_NTS);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        cout << "Error al listar secciones.\n";
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return;
    }

    SQLINTEGER id;
    SQLCHAR nombre[100], jornada[50];

    cout << "\n=== LISTADO DE SECCIONES ===\n";

    while (true) {
        ret = SQLFetch(hStmt);
        if (ret == SQL_NO_DATA) break;

        SQLGetData(hStmt, 1, SQL_C_LONG, &id, 0, NULL);
        SQLGetData(hStmt, 2, SQL_C_CHAR, nombre, sizeof(nombre), NULL);
        SQLGetData(hStmt, 3, SQL_C_CHAR, jornada, sizeof(jornada), NULL);

        cout << "ID: " << id << " | Seccion: " << nombre << " | Jornada: " << jornada << endl;
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

void insertarSeccion() {
    string nombre, jornada;

    cout << "\n=== REGISTRAR SECCION ===\n";
    cout << "Nombre: ";
    cin >> nombre;
    cout << "Jornada: ";
    cin >> jornada;

    string query =
        "INSERT INTO universidad.dbo.secciones (nombre, jornada) VALUES ('" +
        nombre + "','" + jornada + "')";

    ejecutarSQL(query, "Seccion guardada correctamente.", "Error al guardar seccion.");
}

void modificarSeccion() {
    int id;
    string nombre, jornada;

    cout << "\n=== MODIFICAR SECCION ===\n";
    cout << "ID de la seccion: ";
    cin >> id;
    cout << "Nuevo nombre: ";
    cin >> nombre;
    cout << "Nueva jornada: ";
    cin >> jornada;

    string query =
        "UPDATE universidad.dbo.secciones SET nombre='" + nombre +
        "', jornada='" + jornada +
        "' WHERE id=" + to_string(id);

    ejecutarSQL(query, "Seccion modificada correctamente.", "Error al modificar seccion.");
}

void eliminarSeccion() {
    int id;

    cout << "\n=== ELIMINAR SECCION ===\n";
    cout << "ID de la seccion: ";
    cin >> id;

    string query = "DELETE FROM universidad.dbo.secciones WHERE id=" + to_string(id);

    ejecutarSQL(query,
                "Seccion eliminada correctamente.",
                "Error al eliminar seccion. Puede tener alumnos asignados.");
}

void menuSecciones() {
    int opcion;

    do {
        cout << "\n===== MODULO SECCIONES =====\n";
        cout << "1. Listar\n";
        cout << "2. Grabar\n";
        cout << "3. Modificar\n";
        cout << "4. Eliminar\n";
        cout << "5. Regresar\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch (opcion) {
            case 1: listarSecciones(); break;
            case 2: insertarSeccion(); break;
            case 3: modificarSeccion(); break;
            case 4: eliminarSeccion(); break;
            case 5: break;
            default: cout << "Opcion invalida.\n";
        }

    } while (opcion != 5);
}

// ================= ALUMNOS - CURSOS =================

void listarAsignacionesCursos() {
    SQLHSTMT hStmt;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    SQLCHAR query[] =
        "SELECT ac.id, a.nombres, a.apellidos, c.nombre "
        "FROM universidad.dbo.alumnos_cursos ac "
        "INNER JOIN universidad.dbo.alumnos a ON ac.alumno_id = a.id "
        "INNER JOIN universidad.dbo.cursos c ON ac.curso_id = c.id";

    SQLRETURN ret = SQLExecDirect(hStmt, query, SQL_NTS);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        cout << "Error al listar asignaciones.\n";
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return;
    }

    SQLINTEGER id;
    SQLCHAR nombres[100], apellidos[100], curso[100];

    cout << "\n=== ASIGNACIONES ALUMNOS - CURSOS ===\n";

    while (true) {
        ret = SQLFetch(hStmt);
        if (ret == SQL_NO_DATA) break;

        SQLGetData(hStmt, 1, SQL_C_LONG, &id, 0, NULL);
        SQLGetData(hStmt, 2, SQL_C_CHAR, nombres, sizeof(nombres), NULL);
        SQLGetData(hStmt, 3, SQL_C_CHAR, apellidos, sizeof(apellidos), NULL);
        SQLGetData(hStmt, 4, SQL_C_CHAR, curso, sizeof(curso), NULL);

        cout << "ID asignacion: " << id
             << " | Alumno: " << nombres << " " << apellidos
             << " | Curso: " << curso << endl;
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

void asignarCursoAlumno() {
    int alumno_id, curso_id;

    cout << "\n=== ASIGNAR CURSO A ALUMNO ===\n";
    cout << "ID del alumno: ";
    cin >> alumno_id;
    cout << "ID del curso: ";
    cin >> curso_id;

    string query =
        "INSERT INTO universidad.dbo.alumnos_cursos (alumno_id, curso_id) VALUES (" +
        to_string(alumno_id) + "," + to_string(curso_id) + ")";

    ejecutarSQL(query,
                "Curso asignado correctamente.",
                "Error al asignar curso. Puede estar repetido o el ID no existe.");
}

void eliminarAsignacionCurso() {
    int id;

    cout << "\n=== ELIMINAR ASIGNACION DE CURSO ===\n";
    cout << "ID de la asignacion: ";
    cin >> id;

    string query =
        "DELETE FROM universidad.dbo.alumnos_cursos WHERE id=" + to_string(id);

    ejecutarSQL(query,
                "Asignacion eliminada correctamente.",
                "Error al eliminar asignacion.");
}

void menuAlumnosCursos() {
    int opcion;

    do {
        cout << "\n===== MODULO ALUMNOS - CURSOS =====\n";
        cout << "1. Listar asignaciones\n";
        cout << "2. Asignar curso\n";
        cout << "3. Eliminar asignacion\n";
        cout << "4. Regresar\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch (opcion) {
            case 1: listarAsignacionesCursos(); break;
            case 2: asignarCursoAlumno(); break;
            case 3: eliminarAsignacionCurso(); break;
            case 4: break;
            default: cout << "Opcion invalida.\n";
        }

    } while (opcion != 4);
}

// ================= ALUMNOS - SECCION =================

void listarAsignacionesSeccion() {
    SQLHSTMT hStmt;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    SQLCHAR query[] =
        "SELECT ase.id, a.nombres, a.apellidos, s.nombre, s.jornada "
        "FROM universidad.dbo.alumnos_seccion ase "
        "INNER JOIN universidad.dbo.alumnos a ON ase.alumno_id = a.id "
        "INNER JOIN universidad.dbo.secciones s ON ase.seccion_id = s.id";

    SQLRETURN ret = SQLExecDirect(hStmt, query, SQL_NTS);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        cout << "Error al listar asignaciones.\n";
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return;
    }

    SQLINTEGER id;
    SQLCHAR nombres[100], apellidos[100], seccion[100], jornada[50];

    cout << "\n=== ASIGNACIONES ALUMNOS - SECCION ===\n";

    while (true) {
        ret = SQLFetch(hStmt);
        if (ret == SQL_NO_DATA) break;

        SQLGetData(hStmt, 1, SQL_C_LONG, &id, 0, NULL);
        SQLGetData(hStmt, 2, SQL_C_CHAR, nombres, sizeof(nombres), NULL);
        SQLGetData(hStmt, 3, SQL_C_CHAR, apellidos, sizeof(apellidos), NULL);
        SQLGetData(hStmt, 4, SQL_C_CHAR, seccion, sizeof(seccion), NULL);
        SQLGetData(hStmt, 5, SQL_C_CHAR, jornada, sizeof(jornada), NULL);

        cout << "ID asignacion: " << id
             << " | Alumno: " << nombres << " " << apellidos
             << " | Seccion: " << seccion
             << " | Jornada: " << jornada << endl;
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

void asignarSeccionAlumno() {
    int alumno_id, seccion_id;

    cout << "\n=== ASIGNAR SECCION A ALUMNO ===\n";
    cout << "ID del alumno: ";
    cin >> alumno_id;
    cout << "ID de la seccion: ";
    cin >> seccion_id;

    string query =
        "INSERT INTO universidad.dbo.alumnos_seccion (alumno_id, seccion_id) VALUES (" +
        to_string(alumno_id) + "," + to_string(seccion_id) + ")";

    ejecutarSQL(query,
                "Seccion asignada correctamente.",
                "Error al asignar seccion. El alumno ya puede tener una seccion.");
}

void modificarSeccionAlumno() {
    int alumno_id, seccion_id;

    cout << "\n=== MODIFICAR SECCION ASIGNADA ===\n";
    cout << "ID del alumno: ";
    cin >> alumno_id;
    cout << "Nuevo ID de seccion: ";
    cin >> seccion_id;

    string query =
        "UPDATE universidad.dbo.alumnos_seccion SET seccion_id=" +
        to_string(seccion_id) +
        " WHERE alumno_id=" + to_string(alumno_id);

    ejecutarSQL(query,
                "Seccion asignada modificada correctamente.",
                "Error al modificar seccion asignada.");
}

void eliminarSeccionAlumno() {
    int alumno_id;

    cout << "\n=== ELIMINAR SECCION ASIGNADA ===\n";
    cout << "ID del alumno: ";
    cin >> alumno_id;

    string query =
        "DELETE FROM universidad.dbo.alumnos_seccion WHERE alumno_id=" +
        to_string(alumno_id);

    ejecutarSQL(query,
                "Seccion asignada eliminada correctamente.",
                "Error al eliminar seccion asignada.");
}

void menuAlumnosSeccion() {
    int opcion;

    do {
        cout << "\n===== MODULO ALUMNOS - SECCION =====\n";
        cout << "1. Listar asignaciones\n";
        cout << "2. Asignar seccion\n";
        cout << "3. Modificar seccion asignada\n";
        cout << "4. Eliminar seccion asignada\n";
        cout << "5. Regresar\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch (opcion) {
            case 1: listarAsignacionesSeccion(); break;
            case 2: asignarSeccionAlumno(); break;
            case 3: modificarSeccionAlumno(); break;
            case 4: eliminarSeccionAlumno(); break;
            case 5: break;
            default: cout << "Opcion invalida.\n";
        }

    } while (opcion != 5);
}

// ================= MENU PRINCIPAL =================

void menuPrincipal() {
    int opcion;

    do {
        cout << "\n===== SISTEMA UNIVERSIDAD =====\n";
        cout << "1. Alumnos\n";
        cout << "2. Cursos\n";
        cout << "3. Secciones\n";
        cout << "4. Alumnos - Cursos\n";
        cout << "5. Alumnos - Seccion\n";
        cout << "6. Salir\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch (opcion) {
            case 1: menuAlumnos(); break;
            case 2: menuCursos(); break;
            case 3: menuSecciones(); break;
            case 4: menuAlumnosCursos(); break;
            case 5: menuAlumnosSeccion(); break;
            case 6: cout << "Saliendo...\n"; break;
            default: cout << "Opcion invalida.\n";
        }

    } while (opcion != 6);
}

int main() {
    conectar();
    menuPrincipal();
    cerrarConexion();

    return 0;
}
