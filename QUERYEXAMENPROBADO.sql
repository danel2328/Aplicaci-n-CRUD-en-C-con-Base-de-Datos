USE universidad;
GO

DROP TABLE IF EXISTS alumnos_seccion;
DROP TABLE IF EXISTS alumnos_cursos;
DROP TABLE IF EXISTS secciones;
DROP TABLE IF EXISTS cursos;
DROP TABLE IF EXISTS alumnos;
GO

CREATE TABLE alumnos (
    id INT IDENTITY(1,1) PRIMARY KEY,
    nombres VARCHAR(100),
    apellidos VARCHAR(100),
    carnet VARCHAR(50)
);

CREATE TABLE cursos (
    id INT IDENTITY(1,1) PRIMARY KEY,
    nombre VARCHAR(100),
    codigo VARCHAR(50)
);

CREATE TABLE secciones (
    id INT IDENTITY(1,1) PRIMARY KEY,
    nombre VARCHAR(100),
    jornada VARCHAR(50)
);

CREATE TABLE alumnos_cursos (
    id INT IDENTITY(1,1) PRIMARY KEY,
    alumno_id INT,
    curso_id INT,
    FOREIGN KEY (alumno_id) REFERENCES alumnos(id),
    FOREIGN KEY (curso_id) REFERENCES cursos(id),
    UNIQUE (alumno_id, curso_id)
);

CREATE TABLE alumnos_seccion (
    id INT IDENTITY(1,1) PRIMARY KEY,
    alumno_id INT UNIQUE,
    seccion_id INT,
    FOREIGN KEY (alumno_id) REFERENCES alumnos(id),
    FOREIGN KEY (seccion_id) REFERENCES secciones(id)
);

INSERT INTO alumnos (nombres, apellidos, carnet)
VALUES 
('Alex', 'Perez', 'A001'),
('Carlos', 'Ramirez', 'A002'),
('Maria', 'Lopez', 'A003');

INSERT INTO cursos (nombre, codigo)
VALUES
('Programacion I', 'PROG1'),
('Base de Datos', 'BD1'),
('Matematica', 'MAT1');

INSERT INTO secciones (nombre, jornada)
VALUES
('Seccion A', 'Matutina'),
('Seccion B', 'Vespertina'),
('Seccion C', 'Nocturna');

INSERT INTO alumnos_cursos (alumno_id, curso_id)
VALUES
(1,1),
(2,2),
(3,3);

INSERT INTO alumnos_seccion (alumno_id, seccion_id)
VALUES
(1,1),
(2,2),
(3,3);

SELECT 
    a.id,
    a.nombres,
    a.apellidos,
    a.carnet,
    c.nombre AS curso,
    s.nombre AS seccion
FROM alumnos a
LEFT JOIN alumnos_cursos ac ON a.id = ac.alumno_id
LEFT JOIN cursos c ON ac.curso_id = c.id
LEFT JOIN alumnos_seccion ase ON a.id = ase.alumno_id
LEFT JOIN secciones s ON ase.seccion_id = s.id;