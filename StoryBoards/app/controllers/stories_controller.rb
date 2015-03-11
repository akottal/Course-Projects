class StoriesController < ApplicationController
  before_action :set_story, only: [:show, :edit, :update, :destroy]

  def has_access(story)
    return (current_user.project_id == story.project_id) ||
        (current_user.id == story.developer_id)
  end

  def redirect_if_not_proj_mem(story)
    if !has_access(story)
      dev_redirect
    end
  end

  # GET /stories
  # GET /stories.json
  def index
    if !is_admin
      dev_redirect
    end
    @stories = Story.all
  end

  # GET /stories/1
  # GET /stories/1.json
  def show
    @story = Story.find(params[:id])
    redirect_if_not_proj_mem(@story)
    @developers = Developer.all.select {|dev| dev.story_id == params[:id].to_i}
  end

  # GET /stories/new
  def new
    @story = Story.new
    @user = current_user
    @stages = ['Analysis','Ready For Dev','In Dev','Dev Complete', 'In Test','Complete']
  end

  # GET /stories/1/edit
  def edit
    @story = Story.find(params[:id])
    redirect_if_not_proj_mem(@story)
    @user = current_user
    @stages = ['Analysis','Ready For Dev','In Dev','Dev Complete', 'In Test','Complete']
  end


  def return_hours(stage)
    stages = {'Analysis' => 10 , 'Ready For Dev' => 8,'In Dev' => 6,'Dev Complete' => 4, 'In Test' => 2,'Complete' => 0}
    stages[stage]
  end
  # POST /stories
  # POST /stories.json
  def create
    @story = Story.new(story_params)
    @user = current_user
    @story.developer_id = @user.id
    p = Project.find(@story.project_id)
    bd_arr = p.burndown
    btime = p.btime
    btime = btime==nil ? 0 : btime + 1
    if !bd_arr.empty?
      bd_arr << (bd_arr[-1] + return_hours(@story.stage))
    else
      bd_arr << return_hours(@story.stage)
    end

    respond_to do |format|
      if @story.save
        p.update_attributes(:burndown => bd_arr)
        p.update_attributes(:btime => btime)
        format.html { redirect_to @story, notice: 'Story was successfully created.' }
        format.json { render :show, status: :created, location: @story }
      else
        format.html { render :new }
        format.json { render json: @story.errors, status: :unprocessable_entity }
      end
    end
  end

  # PATCH/PUT /stories/1
  # PATCH/PUT /stories/1.json
  def update
    old_stage = Story.find(params[:id]).stage
    new_stage = params[:story][:stage]
    p = Project.find(@story.project_id)
    #cum_hours = return_hours(new_stage)
    cum_hours = return_hours(new_stage) - return_hours(old_stage)
    bd_arr = p.burndown
    btime = p.btime
    btime = btime==nil ? 0 : btime + 1
    if bd_arr.empty?
      bd_arr << (bd_arr[-1] + cum_hours)
    else
      bd_arr << cum_hours
    end
    respond_to do |format|
      if @story.update(story_params)
        p.update_attributes(:burndown => bd_arr)
        p.update_attributes(:btime => btime)
        format.html { redirect_to @story, notice: 'Story was successfully updated.' }
        format.json { render :show, status: :ok, location: @story }
      else
        format.html { render :edit }
        format.json { render json: @story.errors, status: :unprocessable_entity }
      end
    end
  end

  # DELETE /stories/1
  # DELETE /stories/1.json
  def destroy
    d = Developer.all.select{|dev| dev.story_id == params[:id]}
    d.each.update_attributes(:story_id => nil) if !d.empty?

    @story.destroy
    respond_to do |format|
      format.html { redirect_to :back, notice: 'Story was successfully destroyed.' }
      format.json { head :no_content }
    end
  end

  def sign_up
    @user = current_user
    @story = Story.find(params[:id])
    @developers = Developer.all.select{|dev| dev.story_id == params[:id].to_i}
    if @developers.size < 2
      respond_to do |format|
        if @user.update_attributes(:story_id => params[:id].to_i)
          format.html { redirect_to @story, notice: 'You signed up for this story. Any existing signed up story will be replaced by this story.'}
          format.json { render :show, status: :ok, location: @story }
        else
          format.html { render :show }
          format.json { render json: @story.errors, status: :unprocessable_entity }
        end
      end
    else
      i = rand > 0.5 ? 1 : 0
      respond_to do |format|
        if @developers[i].update_attributes(:story_id => nil) && @user.update_attributes(:story_id => params[:id].to_i)
          format.html { redirect_to @story, notice: 'You signed up for this story.Any previous signed up story is replaced by this story.'}
          format.json { render :show, status: :ok, location: @story }
        else
          format.html { render :show }
          format.json { render json: @story.errors, status: :unprocessable_entity }
        end
      end
    end
  end

  private
    # Use callbacks to share common setup or constraints between actions.
    def set_story
      @story = Story.find(params[:id])
    end

    # Never trust parameters from the scary internet, only allow the white list through.
    def story_params
      params.require(:story).permit(:title,:description,:point_value,:stage,:project_id)
    end
end
