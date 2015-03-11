# encoding: UTF-8
# This file is auto-generated from the current state of the database. Instead
# of editing this file, please use the migrations feature of Active Record to
# incrementally modify your database, and then regenerate this schema definition.
#
# Note that this schema.rb definition is the authoritative source for your
# database schema. If you need to create the application database on another
# system, you should be using db:schema:load, not running all the migrations
# from scratch. The latter is a flawed and unsustainable approach (the more migrations
# you'll amass, the slower it'll run and the greater likelihood for issues).
#
# It's strongly recommended that you check this file into your version control system.

ActiveRecord::Schema.define(version: 20150213073959) do

  create_table "admins", force: :cascade do |t|
    t.string   "name"
    t.string   "email"
    t.datetime "created_at",      null: false
    t.datetime "updated_at",      null: false
    t.string   "password_digest"
  end

  create_table "developers", force: :cascade do |t|
    t.string   "name"
    t.string   "email"
    t.integer  "story_id"
    t.integer  "project_id"
    t.datetime "created_at",      null: false
    t.datetime "updated_at",      null: false
    t.string   "password_digest"
  end

  create_table "projects", force: :cascade do |t|
    t.string   "title"
    t.text     "description"
    t.text     "developers"
    t.datetime "created_at",  null: false
    t.datetime "updated_at",  null: false
    t.string   "burndown"
    t.integer  "btime"
  end

  create_table "stories", force: :cascade do |t|
    t.string   "title"
    t.text     "description"
    t.float    "point_value"
    t.string   "stage"
    t.integer  "project_id"
    t.datetime "created_at",   null: false
    t.datetime "updated_at",   null: false
    t.integer  "dev_id"
    t.integer  "developer_id"
  end

end
