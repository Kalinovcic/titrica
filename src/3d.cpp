
inline void normalize3(GLdouble* v)
{
    GLdouble len = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    v[0] /= len;
    v[1] /= len;
    v[2] /= len;
}

void sphere_face(int p_recurse, double radius, GLdouble *a, GLdouble *b, GLdouble *c, bool wireframe)
{
    if (p_recurse > 1)
    {
        GLdouble d[3] = {a[0] + b[0], a[1] + b[1], a[2] + b[2]};
        GLdouble e[3] = {b[0] + c[0], b[1] + c[1], b[2] + c[2]};
        GLdouble f[3] = {c[0] + a[0], c[1] + a[1], c[2] + a[2]};
        normalize3(d);
        normalize3(e);
        normalize3(f);
        sphere_face(p_recurse - 1, radius, a, d, f, wireframe);
        sphere_face(p_recurse - 1, radius, d, b, e, wireframe);
        sphere_face(p_recurse - 1, radius, f, e, c, wireframe);
        sphere_face(p_recurse - 1, radius, f, d, e, wireframe);
    }
    else
    {
        if (wireframe)
        {
            glVertex3d(a[0] * radius, a[1] * radius, a[2] * radius);
            glVertex3d(b[0] * radius, b[1] * radius, b[2] * radius);
            glVertex3d(b[0] * radius, b[1] * radius, b[2] * radius);
            glVertex3d(c[0] * radius, c[1] * radius, c[2] * radius);
            glVertex3d(c[0] * radius, c[1] * radius, c[2] * radius);
            glVertex3d(a[0] * radius, a[1] * radius, a[2] * radius);
        }
        else
        {
            glNormal3dv(a);
            glVertex3d(a[0] * radius, a[1] * radius, a[2] * radius);
            glNormal3dv(b);
            glVertex3d(b[0] * radius, b[1] * radius, b[2] * radius);
            glNormal3dv(c);
            glVertex3d(c[0] * radius, c[1] * radius, c[2] * radius);
        }
   }
}

void sphere(double radius, vec3 position, vec3 color, bool wireframe = false, int recurse = 4)
{
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);

    if (wireframe)
    {
        glColor3f(color.x, color.y, color.z);
        glBegin(GL_LINES);
    }
    else
    {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        GLfloat diffuse[] = { color.x, color.y, color.z };
        GLfloat specular[] = { 1.0f, 1.0f, 1.0f };
        GLfloat shininess[] = { 127 };

        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);

        glBegin(GL_TRIANGLES);
    }

    GLdouble a[] = { 1,  0,  0};
    GLdouble b[] = { 0,  0, -1};
    GLdouble c[] = {-1,  0,  0};
    GLdouble d[] = { 0,  0,  1};
    GLdouble e[] = { 0,  1,  0};
    GLdouble f[] = { 0, -1,  0};

    sphere_face(recurse, radius, d, a, e, wireframe);
    sphere_face(recurse, radius, a, b, e, wireframe);
    sphere_face(recurse, radius, b, c, e, wireframe);
    sphere_face(recurse, radius, c, d, e, wireframe);
    sphere_face(recurse, radius, a, d, f, wireframe);
    sphere_face(recurse, radius, b, a, f, wireframe);
    sphere_face(recurse, radius, c, b, f, wireframe);
    sphere_face(recurse, radius, d, c, f, wireframe);

    glEnd();
    glPopMatrix();

    if (!wireframe)
    {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
    }
}

void rope(vec3 a, vec3 b, double radius, vec3 color)
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat diffuse[] = { color.x, color.y, color.z };
    GLfloat specular[] = { 1.0f, 1.0f, 1.0f };
    GLfloat shininess[] = { 127 };

    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);

    vec3   tangent = normalize(cross(a - b, vec3(1, 1, 1)));
    vec3 bitangent = normalize(cross(tangent, a - b));

    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 64; i++)
    {
        double a1 = (i + 0) / 64.0 * 2 * M_PI;
        double a2 = (i + 1) / 64.0 * 2 * M_PI;
        float x1 = cosf(a1);
        float x2 = cosf(a2);
        float y1 = sinf(a1);
        float y2 = sinf(a2);

        vec3 n1 = tangent * x1 + bitangent * y1;
        vec3 n2 = tangent * x2 + bitangent * y2;
        vec3 p1a = a + (float) radius * n1;
        vec3 p1b = b + (float) radius * n1;
        vec3 p2a = a + (float) radius * n2;
        vec3 p2b = b + (float) radius * n2;

        glNormal3d(n1.x, n1.y, n1.z);
        glVertex3d(p1a.x, p1a.y, p1a.z);
        glNormal3d(n2.x, n2.y, n2.z);
        glVertex3d(p2a.x, p2a.y, p2a.z);
        glNormal3d(n2.x, n2.y, n2.z);
        glVertex3d(p2b.x, p2b.y, p2b.z);

        glNormal3d(n1.x, n1.y, n1.z);
        glVertex3d(p1a.x, p1a.y, p1a.z);
        glNormal3d(n2.x, n2.y, n2.z);
        glVertex3d(p2b.x, p2b.y, p2b.z);
        glNormal3d(n1.x, n1.y, n1.z);
        glVertex3d(p1b.x, p1b.y, p1b.z);
    }

    glEnd();

    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);


    sphere(radius, a, color, false);
    sphere(radius, b, color, false);
}

void box(vec3 position, vec3 size, vec3 color, bool wireframe = false)
{
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    glScalef(size.x, size.y, size.z);

    int n = 6 * 4;
    vec3 vertices[] = {
        { 0.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f },
        { 1.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f },
        { 1.0f, 0.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f },
        { 0.0f, 1.0f, 1.0f },
        { 0.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 1.0f, 0.0f },
        { 1.0f, 1.0f, 0.0f },
        { 1.0f, 1.0f, 1.0f },
        { 0.0f, 1.0f, 1.0f },
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f },
        { 1.0f, 0.0f, 0.0f },
        { 1.0f, 1.0f, 0.0f },
        { 1.0f, 1.0f, 1.0f },
        { 1.0f, 0.0f, 1.0f },
    };
    vec3 normals[] = {
        { 0.0f, 0.0f, -1.0f },
        { 0.0f, 0.0f, +1.0f },
        { 0.0f, -1.0f, 0.0f },
        { 0.0f, +1.0f, 0.0f },
        { -1.0f, 0.0f, 0.0f },
        { +1.0f, 0.0f, 0.0f },
    };

    if (wireframe)
    {
        glColor3f(color.x, color.y, color.z);
        glBegin(GL_LINES);
        for (int i = 0; i < n; i += 4)
        {
            for (int j = 0; j < 4; j++)
            {
                auto a = vertices[i + j];
                auto b = vertices[i + (j + 1) % 4];
                glVertex3f(a.x, a.y, a.z);
                glVertex3f(b.x, b.y, b.z);
            }
        }
        glEnd();
    }
    else
    {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        GLfloat diffuse[] = { color.x, color.y, color.z };
        GLfloat specular[] = { 1.0f, 1.0f, 1.0f };
        GLfloat shininess[] = { 127 };

        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);

        glBegin(GL_QUADS);
        for (int i = 0; i < n; i += 4)
        {
            auto a = normals[i / 4];
            glNormal3f(a.x, a.y, a.z);
            for (int j = 0; j < 4; j++)
            {
                auto a = vertices[i + j];
                glVertex3f(a.x, a.y, a.z);
            }
        }
        glEnd();

        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
    }

    glPopMatrix();
}

void render_3d_test()
{
    GLfloat light_position[] = { 0, 0, 100, 0 };
    GLfloat specular_light[] = { 1.0, 1.0, 1.0 };
    GLfloat ambient_light[] = { 0.2, 0.2, 0.2 };
    GLfloat diffuse_light[] = { 1.0, 1.0, 1.0 };

    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH); // glShadeModel(GL_FLAT);

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular_light);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50.0, window_width / (float) window_height, 0.1, 200.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float camera_radius = camera_zoom;
    camera_position.y = sin(camera_angle_vertical) * camera_radius;
    float layer_radius = abs(cos(camera_angle_vertical) * camera_radius);
    camera_position.x = sin(camera_angle_horizontal) * layer_radius;
    camera_position.z = cos(camera_angle_horizontal) * layer_radius;
    gluLookAt(camera_position.x, camera_position.y, camera_position.z,   0, 0, 0,   0, 1, 0);

    /*glColor3f(0.5, 0.5, 0.5);
    glBegin(GL_LINES);
    if (show_grid)
    {
        for (int x = -20; x <= 20; x++)
        {
            if (x == 0) continue;
            glVertex3f(x, 0, -21);
            glVertex3f(x, 0,  21);
        }
        for (int z = -20; z <= 20; z++)
        {
            if (z == 0) continue;
            glVertex3f(-21, 0, z);
            glVertex3f( 21, 0, z);
        }
    }
    glColor3f(1, 0, 0);
    glVertex3f(-21, 0, 0);
    glVertex3f( 21, 0, 0);
    glColor3f(0, 1, 0);
    glVertex3f(0, -6, 0);
    glVertex3f(0,  6, 0);
    glColor3f(0, 0, 1);
    glVertex3f(0, 0, -21);
    glVertex3f(0, 0,  21);
    glEnd();*/


    double rope_radius = 0.05;
    double stalak_radius = 0.1;
    vec3 stalak_color = vec3(0.4, 0.4, 0.4);
    vec3 rope_color = vec3(0xD4 / 255.0f, 0xC4 / 255.0f, 0x85 / 255.0f);
    vec3 ball_color = vec3(1, 0, 0);

    float time = GetTime();
    float g = 9.81;
    float T1 = 2 * M_PI * sqrt(l1 / g);
    float T2 = 2 * M_PI * sqrt(l2 / g);
    float Euk = A * A * m1 * g / l1 * 0.5;
    float E1 = Euk * (cos(0.3 * time) * 0.5 + 0.5);
    float E2 = Euk - E1;
    float A1 = sqrt(2 * E1 * l1 / m1 / g);
    float A2 = sqrt(2 * E2 * l2 / m2 / g);

    float x1 = cos(time * T1) * A1;
    float x2 = cos(time * T2) * A2;
    float y1 = sqrt(l1 * l1 - x1 * x1);
    float y2 = sqrt(l2 * l2 - x2 * x2);

    vec3 rope_left = vec3(0, -y1, x1);
    vec3 rope_right = vec3(0, -y2, x2);

    vec3 left_ground1 = vec3(-4, -2, 1);
    vec3 left_ground2 = vec3(-4, -2, -1);
    vec3 left_end = vec3(-4, 3, 0);
    vec3 left_root = vec3(-1.5, 2, x1 * 0.15);
    vec3 left = left_root + rope_left;
    vec3 right_ground1 = vec3(4, -2, 1);
    vec3 right_ground2 = vec3(4, -2, -1);
    vec3 right_end = vec3(4, 3, 0);
    vec3 right_root = vec3(1.5, 2, x2 * 0.15);
    vec3 right = right_root + rope_right;

    rope(left_end, left_ground1, stalak_radius, stalak_color);
    rope(left_end, left_ground2, stalak_radius, stalak_color);
    rope(left_ground1, left_ground2, stalak_radius, stalak_color);

    rope(right_end, right_ground1, stalak_radius, stalak_color);
    rope(right_end, right_ground2, stalak_radius, stalak_color);
    rope(right_ground1, right_ground2, stalak_radius, stalak_color);

    rope(left_root, right_root, rope_radius, rope_color);
    rope(left_end, left_root, rope_radius, rope_color);
    rope(left_root, left, rope_radius, rope_color);
    rope(right_end, right_root, rope_radius, rope_color);
    rope(right_root, right, rope_radius, rope_color);

    sphere(0.25, left, ball_color);
    sphere(0.25, right, ball_color);


    glDisable(GL_DEPTH_TEST);
}
