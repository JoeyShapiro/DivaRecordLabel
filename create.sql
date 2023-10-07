-- auto-generated definition
create table scores
(
    id          integer                             not null
        primary key autoincrement
        unique,
    date        timestamp default CURRENT_TIMESTAMP not null,
    pv_id       integer                             not null,
    title       text                                not null,
    difficulty  text                                not null,
    total_score integer                             not null,
    completion  real                                not null,
    grade       text                                not null,
    combo       integer                             not null,
    cool        integer                             not null,
    fine        integer                             not null,
    safe        integer                             not null,
    sad         integer                             not null,
    worst       integer                             not null
);
