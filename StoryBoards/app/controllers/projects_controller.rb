require 'google_chart'

class ProjectsController < ApplicationController
  before_action :set_project, only: [:show, :edit, :update, :destroy]
  before_action :redirect_if_no_access, only: [:new, :index,:show, :edit, :update, :destroy]

  #
  # def has_access(project)
  #   return is_admin || is_proj_mem(project)
  # end
  #
  # def is_proj_mem(project)
  #   return current_user.project_id == project.id
  # end


  def redirect_if_no_access
    @project = Project.find(params[:id]) if params[:id] != nil
    # if !has_access(@project)
    #   dev_redirect
    # end
    dev_redirect unless (is_admin || ( params[:id] != nil && @project.id == current_user.project_id))
  end

  # GET /projects
  # GET /projects.json
  def index
    @projects = Project.all
    @developers = Developer.all.select {|dev| dev.project_id == 1}
  end

  def search
    @stories = Story.search(params[:parent_id],params[:q])
  end
  ## Search is moved to story model
  # def search
  #   @project = Project.find(params[:parent_id])
  #   @stories = Story.all.select { |story| ((story.title =~ /#{params[:q]}/i) ||(story.description =~ /#{params[:q]}/i) ) && (story.project_id == @project.id)}
  # end

  # GET /projects/1
  # GET /projects/1.json
  def show
    @project = Project.find(params[:id] )
    @developers = Developer.all.select {|dev| dev.project_id == params[:id].to_i}
    @stories = Story.all.select {|story| story.project_id == params[:id].to_i}
    @stages = ['Analysis','Ready For Dev','In Dev','Dev Complete', 'In Test','Complete']
    @totals = {}
    @stages.each {|stage| @totals[stage] = @stories.select {|story| story.stage == stage}.inject(0) { |sum, story| sum + story.point_value }}
    bd_arr = @project.burndown
    btime = @project.btime
    lc = GoogleChart::LineChart.new("400x200", "Story Burn Down Chart", false)
    lc.data "Progress", bd_arr, '00ff00'
    lc.axis :y, :range => [0,bd_arr.max], :font_size => 10, :alignment => :center
    lc.axis :x, :range => [0,btime], :font_size => 10, :alignment => :center
    @graph = lc.to_url
  end

  # GET /projects/new
  def new
    @project = Project.new
  end

  # GET /projects/1/edit
  def edit
    redirect_if_not_admin
    redirect_if_no_access
    #redirect_to developer_path(current_user) unless has_access(@project)
  end

  # POST /projects
  # POST /projects.json
  def create
    @project = Project.new(project_params)

    respond_to do |format|
      if @project.save
        format.html { redirect_to admins_path, notice: 'Project was successfully created.' }
        format.json { render :show, status: :created, location: @project }
      else
        format.html { render :new }
        format.json { render json: @project.errors, status: :unprocessable_entity }
      end
    end
  end

  # PATCH/PUT /projects/1
  # PATCH/PUT /projects/1.json
  def update
    respond_to do |format|
      if @project.update(project_params)
        format.html { redirect_to admins_path, notice: 'Project was successfully updated.' }
        format.json { render :show, status: :ok, location: @project }
      else
        format.html { render :edit }
        format.json { render json: @project.errors, status: :unprocessable_entity }
      end
    end
  end

  # DELETE /projects/1
  # DELETE /projects/1.json
  def destroy
    @project.destroy
    respond_to do |format|
      format.html { redirect_to admins_path, notice: 'Project was successfully destroyed.' }
      format.json { head :no_content }
    end
  end

  private
    # Use callbacks to share common setup or constraints between actions.
    def set_project
      @project = Project.find(params[:id])
    end

    # Never trust parameters from the scary internet, only allow the white list through.
    def project_params
      params.require(:project).permit(:title, :description, :developer)
    end
end
